/*
 * This file is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Code by Andrew Tridgell and Siddharth Bharat Purohit
 */
#include "RCOutput.h"
#include <AP_Math/AP_Math.h>
#include <AP_BoardConfig/AP_BoardConfig.h>
#include <AP_HAL/utility/RingBuffer.h>
#include "GPIO.h"

#if HAL_USE_PWM == TRUE

using namespace ChibiOS;

extern const AP_HAL::HAL& hal;

#if HAL_WITH_IO_MCU
#include <AP_IOMCU/AP_IOMCU.h>
extern AP_IOMCU iomcu;
#endif

#define RCOU_SERIAL_TIMING_DEBUG 0

struct RCOutput::pwm_group RCOutput::pwm_group_list[] = { HAL_PWM_GROUPS };
struct RCOutput::irq_state RCOutput::irq;

#define NUM_GROUPS ARRAY_SIZE_SIMPLE(pwm_group_list)

// marker for a disabled channel
#define CHAN_DISABLED 255

// #pragma GCC optimize("Og")

/*
  initialise RC output driver
 */
void RCOutput::init()
{
    for (uint8_t i = 0; i < NUM_GROUPS; i++ ) {
        //Start Pwm groups
        pwm_group &group = pwm_group_list[i];
        group.current_mode = MODE_PWM_NORMAL;
        for (uint8_t j = 0; j < 4; j++ ) {
            if (group.chan[j] != CHAN_DISABLED) {
                total_channels = MAX(total_channels, group.chan[j]+1);
                group.ch_mask |= (1U<<group.chan[j]);
            }
        }
        if (group.ch_mask != 0) {
            pwmStart(group.pwm_drv, &group.pwm_cfg);
            group.pwm_started = true;
        }
    }

#if HAL_WITH_IO_MCU
    if (AP_BoardConfig::io_enabled()) {
        iomcu.init();
        // with IOMCU the local (FMU) channels start at 8
        chan_offset = 8;
        total_channels += chan_offset;
    }
#endif
    chMtxObjectInit(&trigger_mutex);

    // setup default output rate of 50Hz
    set_freq(0xFFFF, 50);
}

/*
  setup the output frequency for a group and start pwm output
 */
void RCOutput::set_freq_group(pwm_group &group)
{
    if (mode_requires_dma(group.current_mode)) {
        // speed setup in DMA handler
        return;
    }
    
    uint16_t freq_set = group.rc_frequency;
    uint32_t old_clock = group.pwm_cfg.frequency;
    uint32_t old_period = group.pwm_cfg.period;

    if (freq_set > 400 || group.current_mode == MODE_PWM_ONESHOT125) {
        // use a 8MHz clock for higher frequencies or for
        // oneshot125. Using 8MHz for oneshot125 results in the full
        // 1000 steps for smooth output
        group.pwm_cfg.frequency = 8000000;
    } else if (freq_set <= 400) {
        // use a 1MHz clock
        group.pwm_cfg.frequency = 1000000;
    }

    // check if the frequency is possible, and keep halving
    // down to 1MHz until it is OK with the hardware timer we
    // are using. If we don't do this we'll hit an assert in
    // the ChibiOS PWM driver on some timers
    PWMDriver *pwmp = group.pwm_drv;
    uint32_t psc = (pwmp->clock / pwmp->config->frequency) - 1;
    while ((psc > 0xFFFF || ((psc + 1) * pwmp->config->frequency) != pwmp->clock) &&
           group.pwm_cfg.frequency > 1000000) {
        group.pwm_cfg.frequency /= 2;
        psc = (pwmp->clock / pwmp->config->frequency) - 1;
    }

    if (group.current_mode == MODE_PWM_ONESHOT ||
        group.current_mode == MODE_PWM_ONESHOT125) {
        // force a period of 0, meaning no pulses till we trigger
        group.pwm_cfg.period = 0;
    } else {
        group.pwm_cfg.period = group.pwm_cfg.frequency/freq_set;
    }
    
    bool force_reconfig = false;
    for (uint8_t j=0; j<4; j++) {
        if (group.pwm_cfg.channels[j].mode == PWM_OUTPUT_ACTIVE_LOW) {
            group.pwm_cfg.channels[j].mode = PWM_OUTPUT_ACTIVE_HIGH;
            force_reconfig = true;
        }
    }

    if (old_clock != group.pwm_cfg.frequency ||
        old_period != group.pwm_cfg.period ||
        !group.pwm_started ||
        force_reconfig) {
        // we need to stop and start to setup the new clock
        if (group.pwm_started) {
            pwmStop(group.pwm_drv);
        }
        pwmStart(group.pwm_drv, &group.pwm_cfg);
        group.pwm_started = true;
    }
    pwmChangePeriod(group.pwm_drv, group.pwm_cfg.period);
}

/*
  set output frequency in HZ for a set of channels given by a mask
 */
void RCOutput::set_freq(uint32_t chmask, uint16_t freq_hz)
{
    //check if the request spans accross any of the channel groups
    uint8_t update_mask = 0;

#if HAL_WITH_IO_MCU
    if (AP_BoardConfig::io_enabled()) {
        // change frequency on IOMCU
        iomcu.set_freq(chmask, freq_hz);
    }
#endif

    // convert to a local (FMU) channel mask
    chmask >>= chan_offset;
    if (chmask == 0) {
        return;
    }

    /*
      we enable the new frequency on all groups that have one
      of the requested channels. This means we may enable high
      speed on some channels that aren't requested, but that
      is needed in order to fly a vehicle such a a hex
      multicopter properly
    */
    for (uint8_t i = 0; i < NUM_GROUPS; i++ ) {
        // greater than 400 doesn't give enough room at higher periods for
        // the down pulse. This still allows for high rate with oneshot and dshot.
        pwm_group &group = pwm_group_list[i];
        uint16_t group_freq = freq_hz;
        if (group_freq > 400 && group.current_mode != MODE_PWM_BRUSHED) {
            group_freq = 400;
        }
        if ((group.ch_mask & chmask) != 0) {
            group.rc_frequency = group_freq;
            set_freq_group(group);
            update_mask |= group.ch_mask;
        }
        if (group_freq > 50) {
            fast_channel_mask |= group.ch_mask;
        }
    }
    if (chmask != update_mask) {
        hal.console->printf("RCOutput: Failed to set PWM frequency req %x set %x\n", (unsigned)chmask, (unsigned)update_mask);
    }
}

/*
  set default output rate
 */
void RCOutput::set_default_rate(uint16_t freq_hz)
{
#if HAL_WITH_IO_MCU
    if (AP_BoardConfig::io_enabled()) {
        iomcu.set_default_rate(freq_hz);
    }
#endif
    for (uint8_t i = 0; i < NUM_GROUPS; i++ ) {
        pwm_group &group = pwm_group_list[i];
        if ((group.ch_mask & fast_channel_mask) || group.ch_mask == 0) {
            // don't change fast channels
            continue;
        }
        group.pwm_cfg.period = group.pwm_cfg.frequency/freq_hz;
        if (group.pwm_started) {
            pwmChangePeriod(group.pwm_drv, group.pwm_cfg.period);
        }
    }
}

uint16_t RCOutput::get_freq(uint8_t chan)
{
    if (chan >= total_channels) {
        return 0;
    }
#if HAL_WITH_IO_MCU
    if (chan < chan_offset) {
        return iomcu.get_freq(chan);
    }
#endif
    chan -= chan_offset;

    for (uint8_t i = 0; i < NUM_GROUPS; i++ ) {
        pwm_group &group = pwm_group_list[i];
        for (uint8_t j = 0; j < 4; j++) {
            if (group.chan[j] == chan) {
                return group.pwm_drv->config->frequency / group.pwm_drv->period;
            }
        }
    }
    // assume 50Hz default
    return 50;
}

void RCOutput::enable_ch(uint8_t chan)
{
    if (chan >= total_channels) {
        return;
    }
    if (chan < chan_offset) {
        return;
    }
    chan -= chan_offset;

    for (uint8_t i = 0; i < NUM_GROUPS; i++ ) {
        pwm_group &group = pwm_group_list[i];
        for (uint8_t j = 0; j < 4; j++) {
            if ((group.chan[j] == chan) && !(en_mask & 1<<chan)) {
                en_mask |= 1<<chan;
            }
        }
    }
}

void RCOutput::disable_ch(uint8_t chan)
{
    if (chan >= total_channels) {
        return;
    }
    if (chan < chan_offset) {
        return;
    }
    chan -= chan_offset;

    for (uint8_t i = 0; i < NUM_GROUPS; i++ ) {
        pwm_group &group = pwm_group_list[i];
        for (uint8_t j = 0; j < 4; j++) {
            if (group.chan[j] == chan) {
                pwmDisableChannel(group.pwm_drv, j);
                en_mask &= ~(1<<chan);
            }
        }
    }
}

void RCOutput::write(uint8_t chan, uint16_t period_us)
{
    if (chan >= total_channels) {
        return;
    }
    last_sent[chan] = period_us;

#if HAL_WITH_IO_MCU
    // handle IO MCU channels
    if (AP_BoardConfig::io_enabled()) {
        uint16_t io_period_us = period_us;
        if (iomcu_oneshot125) {
            // the iomcu only has one oneshot setting, so we need to scale by a factor
            // of 8 here for oneshot125
            io_period_us /= 8;
        }
        iomcu.write_channel(chan, io_period_us);
    }
#endif
    if (chan < chan_offset) {
        return;
    }
    chan -= chan_offset;

    period[chan] = period_us;
    num_channels = MAX(chan+1, num_channels);
    if (!corked) {
        push_local();
    }
}

/*
  push values to local channels from period[] array
 */
void RCOutput::push_local(void)
{
    if (num_channels == 0) {
        return;
    }
    uint16_t outmask = (1U<<num_channels)-1;
    outmask &= en_mask;

    uint16_t widest_pulse = 0;
    uint8_t need_trigger = 0;

    for (uint8_t i = 0; i < NUM_GROUPS; i++ ) {
        pwm_group &group = pwm_group_list[i];
        if (serial_group == &group) {
            continue;
        }
        if (!group.pwm_started) {
            continue;
        }
        for (uint8_t j = 0; j < 4; j++) {
            uint8_t chan = group.chan[j];
            if (chan == CHAN_DISABLED) {
                continue;
            }
            if (outmask & (1UL<<chan)) {
                uint32_t period_us = period[chan];
                if (group.current_mode == MODE_PWM_BRUSHED) {
                    if (period_us <= _esc_pwm_min) {
                        period_us = 0;
                    } else if (period_us >= _esc_pwm_max) {
                        period_us = PWM_FRACTION_TO_WIDTH(group.pwm_drv, 1, 1);
                    } else {
                        period_us = PWM_FRACTION_TO_WIDTH(group.pwm_drv,\
                               (_esc_pwm_max - _esc_pwm_min), (period_us - _esc_pwm_min));
                    }
                    pwmEnableChannel(group.pwm_drv, j, period_us);
                } else if (group.current_mode == MODE_PWM_ONESHOT125) {
                    // this gives us a width in 125 ns increments, giving 1000 steps over the 125 to 250 range
                    uint32_t width = ((group.pwm_cfg.frequency/1000000U) * period_us) / 8U;
                    pwmEnableChannel(group.pwm_drv, j, width);                    
                } else if (group.current_mode < MODE_PWM_DSHOT150) {
                    uint32_t width = (group.pwm_cfg.frequency/1000000U) * period_us;
                    pwmEnableChannel(group.pwm_drv, j, width);
                } else if (group.current_mode >= MODE_PWM_DSHOT150 && group.current_mode <= MODE_PWM_DSHOT1200) {
                    // set period_us to time for pulse output, to enable very fast rates
                    period_us = dshot_pulse_time_us;
                }
                if (period_us > widest_pulse) {
                    widest_pulse = period_us;
                }
                if (group.current_mode == MODE_PWM_ONESHOT ||
                    group.current_mode == MODE_PWM_ONESHOT125 ||
                    (group.current_mode >= MODE_PWM_DSHOT150 &&
                     group.current_mode <= MODE_PWM_DSHOT1200)) {
                    need_trigger |= (1U<<i);
                }
            }
        }
    }

    if (widest_pulse > 2300) {
        widest_pulse = 2300;
    }
    trigger_widest_pulse = widest_pulse;

    trigger_groupmask = need_trigger;

    if (trigger_groupmask) {
        trigger_groups();
    }
}

uint16_t RCOutput::read(uint8_t chan)
{
    if (chan >= total_channels) {
        return 0;
    }
#if HAL_WITH_IO_MCU
    if (chan < chan_offset) {
        return iomcu.read_channel(chan);
    }
#endif
    chan -= chan_offset;
    return period[chan];
}

void RCOutput::read(uint16_t* period_us, uint8_t len)
{
    if (len > total_channels) {
        len = total_channels;
    }
#if HAL_WITH_IO_MCU
    for (uint8_t i=0; i<MIN(len, chan_offset); i++) {
        period_us[i] = iomcu.read_channel(i);
    }
#endif
    if (len <= chan_offset) {
        return;
    }
    len -= chan_offset;
    period_us += chan_offset;

    memcpy(period_us, period, len*sizeof(uint16_t));
}

uint16_t RCOutput::read_last_sent(uint8_t chan)
{
    if (chan >= total_channels) {
        return 0;
    }
    return last_sent[chan];
}

void RCOutput::read_last_sent(uint16_t* period_us, uint8_t len)
{
    if (len > total_channels) {
        len = total_channels;
    }
    for (uint8_t i=0; i<len; i++) {
        period_us[i] = read_last_sent(i);
    }
}

/*
  does an output mode require the use of the UP DMA channel?
 */
bool RCOutput::mode_requires_dma(enum output_mode mode) const
{
    switch (mode) {
    case MODE_PWM_DSHOT150:
    case MODE_PWM_DSHOT300:
    case MODE_PWM_DSHOT600:
    case MODE_PWM_DSHOT1200:
        return true;
    default:
        break;
    }
    return false;
}

/*
  setup a group for DMA output at a given bitrate. The bit_width is
  the value for a pulse width in the DMA buffer for a full bit.

  This is used for both DShot and serial output
 */
bool RCOutput::setup_group_DMA(pwm_group &group, uint32_t bitrate, uint32_t bit_width, bool active_high)
{
    if (!group.dma_buffer) {
        group.dma_buffer = (uint32_t *)hal.util->malloc_type(dshot_buffer_length, AP_HAL::Util::MEM_DMA_SAFE);
        if (!group.dma_buffer) {
            return false;
        }
    }
    // for dshot we setup for DMAR based output
    if (!group.dma) {
        group.dma = STM32_DMA_STREAM(group.dma_up_stream_id);
        group.dma_handle = new Shared_DMA(group.dma_up_stream_id, SHARED_DMA_NONE,
                                          FUNCTOR_BIND_MEMBER(&RCOutput::dma_allocate, void, Shared_DMA *),
                                          FUNCTOR_BIND_MEMBER(&RCOutput::dma_deallocate, void, Shared_DMA *));
        if (!group.dma_handle) {
            return false;
        }
    }

    // hold the lock during setup, to ensure there isn't a DMA operation ongoing
    group.dma_handle->lock();
    
    // configure timer driver for DMAR at requested rate
    if (group.pwm_started) {
        pwmStop(group.pwm_drv);
        group.pwm_started = false;
    }

    // adjust frequency to give an allowed value given the
    // clock. There is probably a better way to do this
    uint32_t clock_hz = group.pwm_drv->clock;
    uint32_t target_frequency = bitrate * bit_width;
    uint32_t prescaler = clock_hz / target_frequency;
    while ((clock_hz / prescaler) * prescaler != clock_hz && prescaler <= 0x8000) {
        prescaler++;
    }
    uint32_t freq = clock_hz / prescaler;
    if (prescaler > 0x8000) {
        group.dma_handle->unlock();
        return false;
    }

    group.pwm_cfg.frequency = freq;
    group.pwm_cfg.period = bit_width;
    group.pwm_cfg.dier = TIM_DIER_UDE;
    group.pwm_cfg.cr2 = 0;
    group.bit_width_mul = (freq + (target_frequency/2)) / target_frequency;

    for (uint8_t j=0; j<4; j++) {
        if (group.pwm_cfg.channels[j].mode != PWM_OUTPUT_DISABLED) {
            group.pwm_cfg.channels[j].mode = active_high?PWM_OUTPUT_ACTIVE_HIGH:PWM_OUTPUT_ACTIVE_LOW;
        }
    }
    
    pwmStart(group.pwm_drv, &group.pwm_cfg);
    group.pwm_started = true;
    
    for (uint8_t j=0; j<4; j++) {
        if (group.chan[j] != CHAN_DISABLED) {
            pwmEnableChannel(group.pwm_drv, j, 0);
        }
    }
    group.dma_handle->unlock();
    return true;
}


/*
  setup output mode for a group, using group.current_mode. Used to restore output 
  after serial operations
 */
void RCOutput::set_group_mode(pwm_group &group)
{
    if (group.pwm_started) {
        pwmStop(group.pwm_drv);
        group.pwm_started = false;
    }
    
    switch (group.current_mode) {
    case MODE_PWM_BRUSHED:
        // force zero output initially
        for (uint8_t i=0; i<4; i++) {
            if (group.chan[i] == CHAN_DISABLED) {
                continue;
            }
            uint8_t chan = chan_offset + group.chan[i];
            write(chan, 0);
        }
        break;
        
    case MODE_PWM_DSHOT150 ... MODE_PWM_DSHOT1200: {
        const uint16_t rates[(1 + MODE_PWM_DSHOT1200) - MODE_PWM_DSHOT150] = { 150, 300, 600, 1200 };
        uint32_t rate = rates[uint8_t(group.current_mode - MODE_PWM_DSHOT150)] * 1000UL;
        const uint32_t bit_period = 19;
        
        // configure timer driver for DMAR at requested rate
        if (!setup_group_DMA(group, rate, bit_period, true)) {
            group.current_mode = MODE_PWM_NONE;
            break;
        }

        // calculate min time between pulses
        dshot_pulse_time_us = 1000000UL * dshot_bit_length / rate;
        break;
    }
            
    case MODE_PWM_ONESHOT:
    case MODE_PWM_ONESHOT125:
        // for oneshot we set a period of 0, which results in no pulses till we trigger
        group.pwm_cfg.period = 0;
        group.rc_frequency = 1;
        if (group.pwm_started) {
            pwmChangePeriod(group.pwm_drv, group.pwm_cfg.period);
        }
        break;

    case MODE_PWM_NORMAL:
    case MODE_PWM_NONE:
        // nothing needed
        break;
    }

    set_freq_group(group);
    
    if (group.current_mode != MODE_PWM_NONE &&
        !group.pwm_started) {
        pwmStart(group.pwm_drv, &group.pwm_cfg);
        group.pwm_started = true;
        for (uint8_t j=0; j<4; j++) {
            if (group.chan[j] != CHAN_DISABLED) {
                pwmEnableChannel(group.pwm_drv, j, 0);
            }
        }
    }
}

/*
  setup output mode
 */
void RCOutput::set_output_mode(uint16_t mask, enum output_mode mode)
{
    for (uint8_t i = 0; i < NUM_GROUPS; i++ ) {
        pwm_group &group = pwm_group_list[i];
        if (((group.ch_mask << chan_offset) & mask) == 0) {
            // this group is not affected
            continue;
        }
        if (mode_requires_dma(mode) && !group.have_up_dma) {
            mode = MODE_PWM_NONE;
        }
        if (group.current_mode != mode) {
            group.current_mode = mode;
            set_group_mode(group);
        }
    }
#if HAL_WITH_IO_MCU
    if ((mode == MODE_PWM_ONESHOT ||
         mode == MODE_PWM_ONESHOT125) &&
        (mask & ((1U<<chan_offset)-1)) &&
        AP_BoardConfig::io_enabled()) {
        iomcu_oneshot125 = (mode == MODE_PWM_ONESHOT125);
        // also setup IO to use a 1Hz frequency, so we only get output
        // when we trigger
        iomcu.set_freq(0xFF, 1);
        return iomcu.set_oneshot_mode();
    }
#endif
}

/*
  force the safety switch on, disabling PWM output from the IO board
*/
bool RCOutput::force_safety_on(void)
{
#if HAL_WITH_IO_MCU
    if (AP_BoardConfig::io_enabled()) {
        return iomcu.force_safety_on();
    }
#endif
    return false;
}

/*
  force the safety switch off, enabling PWM output from the IO board
*/
void RCOutput::force_safety_off(void)
{
#if HAL_WITH_IO_MCU
    if (AP_BoardConfig::io_enabled()) {
        iomcu.force_safety_off();
    }
#endif
}

/*
  start corking output
 */
void RCOutput::cork(void)
{
    corked = true;
#if HAL_WITH_IO_MCU
    if (AP_BoardConfig::io_enabled()) {
        iomcu.cork();
    }
#endif
}

/*
  stop corking output
 */
void RCOutput::push(void)
{
    corked = false;
    push_local();
#if HAL_WITH_IO_MCU
    if (AP_BoardConfig::io_enabled()) {
        iomcu.push();
    }
#endif
}

/*
  enable sbus output
 */
bool RCOutput::enable_px4io_sbus_out(uint16_t rate_hz)
{
#if HAL_WITH_IO_MCU
    if (AP_BoardConfig::io_enabled()) {
        return iomcu.enable_sbus_out(rate_hz);
    }
#endif
    return false;
}

/*
  trigger output groups for oneshot or dshot modes
 */
void RCOutput::trigger_groups(void)
{
    if (!chMtxTryLock(&trigger_mutex)) {
        return;
    }
    uint64_t now = AP_HAL::micros64();
    if (now < min_pulse_trigger_us) {
        // guarantee minimum pulse separation
        hal.scheduler->delay_microseconds(min_pulse_trigger_us - now);
    }

    osalSysLock();
    for (uint8_t i = 0; i < NUM_GROUPS; i++) {
        pwm_group &group = pwm_group_list[i];
        if (irq.waiter) {
            // doing serial output, don't send pulses
            continue;
        }
        if (group.current_mode == MODE_PWM_ONESHOT ||
            group.current_mode == MODE_PWM_ONESHOT125) {
            if (trigger_groupmask & (1U<<i)) {
                // this triggers pulse output for a channel group
                group.pwm_drv->tim->EGR = STM32_TIM_EGR_UG;
            }
        }
    }
    osalSysUnlock();

    for (uint8_t i = 0; i < NUM_GROUPS; i++) {
        pwm_group &group = pwm_group_list[i];
        if (serial_group == &group) {
            continue;
        }
        if (group.current_mode >= MODE_PWM_DSHOT150 && group.current_mode <= MODE_PWM_DSHOT1200) {
            dshot_send(group, false);
        }
    }
    
    /*
      calculate time that we are allowed to trigger next pulse
      to guarantee at least a 50us gap between pulses
    */
    min_pulse_trigger_us = AP_HAL::micros64() + trigger_widest_pulse + 50;

    chMtxUnlock(&trigger_mutex);
}

/*
  periodic timer. The only need for a periodic timer is in oneshot
  mode where we want to sustain a minimum output rate for when the
  main loop is busy doing something like gyro calibration

  A mininum output rate helps with some oneshot ESCs
 */

void RCOutput::timer_tick(void)
{
    uint64_t now = AP_HAL::micros64();
    for (uint8_t i = 0; i < NUM_GROUPS; i++ ) {
        pwm_group &group = pwm_group_list[i];
        if (serial_group != &group &&
            group.current_mode >= MODE_PWM_DSHOT150 &&
            group.current_mode <= MODE_PWM_DSHOT1200 &&
            now - group.last_dshot_send_us > 900) {
            // do a blocking send now, to guarantee DShot sends at
            // above 1000 Hz. This makes the protocol more reliable on
            // long cables, and also keeps some ESCs happy that don't
            // like low rates
            dshot_send(group, true);
        }
    }
    if (min_pulse_trigger_us == 0 ||
        serial_group != nullptr) {
        return;
    }
    if (now > min_pulse_trigger_us &&
        now - min_pulse_trigger_us > 4000) {
        // trigger at a minimum of 250Hz
        trigger_groups();
    }
}

/*
  allocate DMA channel
 */
void RCOutput::dma_allocate(Shared_DMA *ctx)
{
    for (uint8_t i = 0; i < NUM_GROUPS; i++ ) {
        pwm_group &group = pwm_group_list[i];
        if (group.dma_handle == ctx) {
            dmaStreamAllocate(group.dma, 10, dma_irq_callback, &group);
        }
    }
}

/*
  deallocate DMA channel
 */
void RCOutput::dma_deallocate(Shared_DMA *ctx)
{
    for (uint8_t i = 0; i < NUM_GROUPS; i++ ) {
        pwm_group &group = pwm_group_list[i];
        if (group.dma_handle == ctx) {
            dmaStreamRelease(group.dma);
        }
    }
}

/*
  create a DSHOT 16 bit packet. Based on prepareDshotPacket from betaflight
 */
uint16_t RCOutput::create_dshot_packet(const uint16_t value, bool telem_request)
{
    uint16_t packet = (value << 1);

    if (telem_request) {
        packet |= 1;
    }

    // compute checksum
    uint16_t csum = 0;
    uint16_t csum_data = packet;
    for (uint8_t i = 0; i < 3; i++) {
        csum ^= csum_data;
        csum_data >>= 4;
    }
    csum &= 0xf;
    // append checksum
    packet = (packet << 4) | csum;

    return packet;
}

/*
  fill in a DMA buffer for dshot
 */
void RCOutput::fill_DMA_buffer_dshot(uint32_t *buffer, uint8_t stride, uint16_t packet, uint16_t clockmul)
{
    const uint32_t DSHOT_MOTOR_BIT_0 = 7 * clockmul;
    const uint32_t DSHOT_MOTOR_BIT_1 = 14 * clockmul;
    for (uint16_t i = 0; i < 16; i++) {
        buffer[i * stride] = (packet & 0x8000) ? DSHOT_MOTOR_BIT_1 : DSHOT_MOTOR_BIT_0;
        packet <<= 1;
    }
}

/*
  send a set of DShot packets for a channel group
  This call be called in blocking mode from the timer, in which case it waits for the DMA lock.
  In normal operation it doesn't wait for the DMA lock.
 */
void RCOutput::dshot_send(pwm_group &group, bool blocking)
{
    if (irq.waiter) {
        // doing serial output, don't send DShot pulses
        return;
    }
    
    if (blocking) {
        group.dma_handle->lock();
    } else {
        if (!group.dma_handle->lock_nonblock()) {
            return;
        }
    }
    for (uint8_t i=0; i<4; i++) {
        uint8_t chan = group.chan[i];
        if (chan != CHAN_DISABLED) {
            uint16_t pwm = period[chan];
            pwm = constrain_int16(pwm, _esc_pwm_min, _esc_pwm_max);
            uint16_t value = 2000UL * uint32_t(pwm - _esc_pwm_min) / uint32_t(_esc_pwm_max - _esc_pwm_min);
            if (value != 0) {
                // dshot values are from 48 to 2047. Zero means off.
                value += 47;
            }
            uint16_t chan_mask = (1U<<chan);
            bool request_telemetry = (telem_request_mask & chan_mask)?true:false;
            uint16_t packet = create_dshot_packet(value, request_telemetry);
            if (request_telemetry) {
                telem_request_mask &= ~chan_mask;
            }
            fill_DMA_buffer_dshot(group.dma_buffer + i, 4, packet, group.bit_width_mul);
        }
    }

    // start sending the pulses out
    send_pulses_DMAR(group, dshot_buffer_length);

    group.last_dshot_send_us = AP_HAL::micros64();
}

/*
  send a series of pulses for a group using DMAR. Pulses must have
  been encoded into the group dma_buffer with interleaving for the 4
  channels in the group
 */
void RCOutput::send_pulses_DMAR(pwm_group &group, uint32_t buffer_length)
{
    /*
      The DMA approach we are using is based on the DMAR method from
      betaflight. We use the TIMn_UP DMA channel for the timer, and
      setup an interleaved set of pulse durations, with a stride of 4
      (for the 4 channels). We use the DMAR register to point the DMA
      engine at the 4 CCR registers of the timer, so it fills in the
      pulse widths for each timer in turn. This means we only use a
      single DMA channel for groups of 4 timer channels. See the "DMA
      address for full transfer TIMx_DMAR" section of the
      datasheet. Many thanks to the betaflight developers for coming
      up with this great method.
     */
    dmaStreamSetPeripheral(group.dma, &(group.pwm_drv->tim->DMAR));
    dmaStreamSetMemory0(group.dma, group.dma_buffer);
    dmaStreamSetTransactionSize(group.dma, buffer_length/sizeof(uint32_t));
    dmaStreamSetFIFO(group.dma, STM32_DMA_FCR_DMDIS | STM32_DMA_FCR_FTH_FULL);
    dmaStreamSetMode(group.dma,
                     STM32_DMA_CR_CHSEL(group.dma_up_channel) |
                     STM32_DMA_CR_DIR_M2P | STM32_DMA_CR_PSIZE_WORD | STM32_DMA_CR_MSIZE_WORD |
                     STM32_DMA_CR_MINC | STM32_DMA_CR_PL(3) |
                     STM32_DMA_CR_TEIE | STM32_DMA_CR_TCIE);

    // setup for 4 burst strided transfers. 0x0D is the register
    // address offset of the CCR registers in the timer peripheral
    group.pwm_drv->tim->DCR = 0x0D | STM32_TIM_DCR_DBL(3);

    dmaStreamEnable(group.dma);
}

/*
  DMA interrupt handler. Used to mark DMA completed for DShot
 */
void RCOutput::dma_irq_callback(void *p, uint32_t flags)
{
    pwm_group *group = (pwm_group *)p;
    dmaStreamDisable(group->dma);
    if (group->in_serial_dma && irq.waiter) {
        // tell the waiting process we've done the DMA
        chSysLockFromISR();
        chEvtSignalI(irq.waiter, serial_event_mask);
        chSysUnlockFromISR();
    } else {
        group->dma_handle->unlock_from_IRQ();
    }
}

/*
  setup for serial output to an ESC using the given
  baudrate. Assumes 1 start bit, 1 stop bit, LSB first and 8
  databits. This is used for passthrough ESC configuration and
  firmware flashing
  
  While serial output is active normal output to the channel group is
  suspended.
*/
bool RCOutput::serial_setup_output(uint8_t chan, uint32_t baudrate)
{
    // account for IOMCU channels
    chan -= chan_offset;
    pwm_group *new_serial_group = nullptr;
    
    // find the channel group
    for (uint8_t i = 0; i < NUM_GROUPS; i++ ) {
        pwm_group &group = pwm_group_list[i];
        if (group.current_mode == MODE_PWM_BRUSHED) {
            // can't do serial output with brushed motors
            continue;
        }
        if (group.ch_mask & (1U<<chan)) {
            if (serial_group && serial_group != &group) {
                // we're changing to a new group, end the previous output
                serial_end();
            }
            new_serial_group = &group;
            for (uint8_t j=0; j<4; j++) {
                if (group.chan[j] == chan) {
                    group.serial.chan = j;
                }
            }
            break;
        }
    }

    if (!new_serial_group) {
        if (serial_group) {
            // shutdown old group
            serial_end();
        }
        return false;
    }

    // setup the group for serial output. We ask for a bit width of 1, which gets modified by the 
    if (!setup_group_DMA(*new_serial_group, baudrate, 10, false)) {
        return false;
    }

    serial_group = new_serial_group;

    // run the thread doing serial IO at highest priority. This is needed to ensure we don't
    // lose bytes when we switch between output and input
    serial_thread = chThdGetSelfX();
    serial_priority  = chThdGetSelfX()->realprio;
    chThdSetPriority(HIGHPRIO);
    
    // remember the bit period for serial_read_byte()
    serial_group->serial.bit_time_us = 1000000UL / baudrate;
    
    // remember the thread that set things up. This is also used to
    // mark the group as doing serial output, so normal output is
    // suspended
    irq.waiter = chThdGetSelfX();

    return true;
}
        

/*
  fill in a DMA buffer for a serial byte, assuming 1 start bit and 1 stop bit
 */
void RCOutput::fill_DMA_buffer_byte(uint32_t *buffer, uint8_t stride, uint8_t b, uint32_t bitval)
{
    const uint32_t BIT_0 = bitval;
    const uint32_t BIT_1 = 0;

    // start bit
    buffer[0] = BIT_0;

    // stop bit
    buffer[9*stride] = BIT_1;

    // 8 data bits
    for (uint8_t i = 0; i < 8; i++) {
        buffer[(1 + i) * stride] = (b & 1) ? BIT_1 : BIT_0;
        b >>= 1;
    }
}


/*
  send one serial byte, blocking call, should be called with the DMA lock held
*/
bool RCOutput::serial_write_byte(uint8_t b)
{
    chEvtGetAndClearEvents(serial_event_mask);
    
    fill_DMA_buffer_byte(serial_group->dma_buffer+serial_group->serial.chan, 4, b, serial_group->bit_width_mul*10);

    serial_group->in_serial_dma = true;
    
    // start sending the pulses out
    send_pulses_DMAR(*serial_group, 10*4*sizeof(uint32_t));

    // wait for the event
    eventmask_t mask = chEvtWaitAnyTimeout(serial_event_mask, MS2ST(2));

    serial_group->in_serial_dma = false;
    
    return (mask & serial_event_mask) != 0;
}

/*
  send a set of serial bytes, blocking call
*/
bool RCOutput::serial_write_bytes(const uint8_t *bytes, uint16_t len)
{
    if (!serial_group) {
        return false;
    }
    serial_group->dma_handle->lock();
    memset(serial_group->dma_buffer, 0, dshot_buffer_length);
    while (len--) {
        if (!serial_write_byte(*bytes++)) {
            serial_group->dma_handle->unlock();
            return false;
        }
    }
    serial_group->dma_handle->unlock();
    // add a small delay for last word of output to have completely
    // finished
    hal.scheduler->delay_microseconds(10);
    return true;
}

/*
  irq handler for bit transition in serial_read_byte()
  This implements a one byte soft serial reader
 */
void RCOutput::serial_bit_irq(void)
{
    uint32_t now = AP_HAL::micros();
    uint8_t bit = palReadLine(irq.line);
    bool send_signal = false;

#if RCOU_SERIAL_TIMING_DEBUG
    palWriteLine(HAL_GPIO_LINE_GPIO55, bit);
#endif
    
    if (irq.nbits == 0 || bit == irq.last_bit) {
        // start of byte, should be low
        if (bit != 0) {
            irq.byteval = 0x200;
            send_signal = true;
        } else {
            irq.nbits = 1;
            irq.byte_start_us = now;
            irq.bitmask = 0;
        }
    } else {
        uint32_t dt = now - irq.byte_start_us;
        uint8_t bitnum = (dt+(irq.bit_time_us/2)) / irq.bit_time_us;

        if (bitnum > 10) {
            bitnum = 10;
        }
        if (!bit) {
            // set the bits that we've processed
            irq.bitmask |= ((1U<<bitnum)-1) & ~((1U<<irq.nbits)-1);
        }
        irq.nbits = bitnum;

        if (irq.nbits == 10) {
            send_signal = true;
            irq.byteval = irq.bitmask & 0x3FF;
            irq.bitmask = 0;
            irq.nbits = 1;
            irq.byte_start_us = now;
        }
    }
    irq.last_bit = bit;
    
    if (send_signal) {
        chSysLockFromISR();
        chEvtSignalI(irq.waiter, serial_event_mask);
        chSysUnlockFromISR();
    }
}


/*
  read a byte from a port, using serial parameters from serial_setup_output()
*/
bool RCOutput::serial_read_byte(uint8_t &b)
{
    bool timed_out = ((chEvtWaitAnyTimeout(serial_event_mask, MS2ST(10)) & serial_event_mask) == 0);

    uint16_t byteval = irq.byteval;

    if (timed_out) {
        // we can accept a byte with a timeout if the last bit was 1
        // and the start bit is set correctly
        if (irq.last_bit == 0) {
            return false;
        }
        byteval = irq.bitmask | 0x200;
    }
    
    if ((byteval & 0x201) != 0x200) {
        // wrong start/stop bits
        return false;
    }

    b = uint8_t(byteval>>1);
    return true;
}

/*
  read a byte from a port, using serial parameters from serial_setup_output()
*/
uint16_t RCOutput::serial_read_bytes(uint8_t *buf, uint16_t len)
{
    if (serial_group == nullptr) {
        return 0;
    }
    pwm_group &group = *serial_group;
    uint8_t chan = group.chan[group.serial.chan];
    uint32_t gpio_mode = PAL_MODE_INPUT | PAL_STM32_OSPEED_LOWEST | PAL_STM32_OTYPE_PUSHPULL | PAL_STM32_PUPDR_PULLUP;
    uint32_t restore_mode = PAL_MODE_ALTERNATE(group.alt_functions[group.serial.chan]) | PAL_STM32_OSPEED_MID2 | PAL_STM32_OTYPE_PUSHPULL;
    uint16_t i = 0;

#ifndef HAL_GPIO_LINE_GPIO50
    // GPIO lines not setup for PWM outputs in hwdef.dat
    return false;
#endif

#if RCOU_SERIAL_TIMING_DEBUG
    hal.gpio->pinMode(54, 1);
    hal.gpio->pinMode(55, 1);
#endif
    
    // assume GPIO mappings for PWM outputs start at 50
    const uint8_t gpio_pin = 50 + chan;
    ((GPIO *)hal.gpio)->_set_mode(gpio_pin, gpio_mode);

    chEvtGetAndClearEvents(serial_event_mask);

    irq.line = group.pal_lines[group.serial.chan];
    irq.nbits = 0;
    irq.bitmask = 0;
    irq.byteval = 0;
    irq.bit_time_us = serial_group->serial.bit_time_us;
    irq.last_bit = 0;
    irq.waiter = chThdGetSelfX();

#if RCOU_SERIAL_TIMING_DEBUG
    palWriteLine(HAL_GPIO_LINE_GPIO54, 1);
#endif
    
    if (!hal.gpio->attach_interrupt(gpio_pin, serial_bit_irq, HAL_GPIO_INTERRUPT_BOTH)) {
#if RCOU_SERIAL_TIMING_DEBUG
        palWriteLine(HAL_GPIO_LINE_GPIO54, 0);
#endif
        return false;
    }
    
    for (i=0; i<len; i++) {
        if (!serial_read_byte(buf[i])) {
            break;
        }
    }

    hal.gpio->attach_interrupt(gpio_pin, nullptr, 0);
    irq.waiter = nullptr;
    
    ((GPIO *)hal.gpio)->_set_mode(gpio_pin, restore_mode);
#if RCOU_SERIAL_TIMING_DEBUG
    palWriteLine(HAL_GPIO_LINE_GPIO54, 0);
#endif
    return i;
}

/*
  end serial output
*/
void RCOutput::serial_end(void)
{
    if (serial_group) {
        pwm_group &group = *serial_group;
        // restore normal output
        if (group.pwm_started) {
            pwmStop(group.pwm_drv);
            group.pwm_started = false;
        }
        set_group_mode(group);
        set_freq_group(group);
        irq.waiter = nullptr;
        if (serial_thread == chThdGetSelfX()) {
            chThdSetPriority(serial_priority);
            serial_thread = nullptr;
        }
    }
    serial_group = nullptr;
}

#endif // HAL_USE_PWM
