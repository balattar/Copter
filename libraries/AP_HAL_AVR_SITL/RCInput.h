
#ifndef __AP_HAL_AVR_SITL_RCINPUT_H__
#define __AP_HAL_AVR_SITL_RCINPUT_H__

#include <AP_HAL_AVR_SITL.h>

class AVR_SITL::SITLRCInput : public AP_HAL::RCInput {
public:
    SITLRCInput(SITL_State *sitlState) {
	    _sitlState = sitlState;
    }
    void init(void* machtnichts);
    uint8_t  valid();
    uint16_t read(uint8_t ch);
    uint8_t read(uint16_t* periods, uint8_t len);

    bool set_overrides(int16_t *overrides, uint8_t len);
    bool set_override(uint8_t channel, int16_t override);
    void clear_overrides();
private:
    SITL_State *_sitlState;

    /* override state */
    uint16_t _override[8];
};

#endif // __AP_HAL_AVR_SITL_RCINPUT_H__

