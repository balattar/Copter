#include "UARTDriver.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include <AP_HAL/AP_HAL.h>

#include "ConsoleDevice.h"
#include "TCPServerDevice.h"
#include "UARTDevice.h"
#include "UARTQFlight.h"
#include "UDPDevice.h"

#include <GCS_MAVLink/GCS.h>

extern const AP_HAL::HAL& hal;

using namespace Linux;

UARTDriver::UARTDriver(bool default_console) :
    device_path(NULL),
    _packetise(false),
    _device{new ConsoleDevice()}
{
    if (default_console) {
        _console = true;
    }
}

/*
  set the tty device to use for this UART
 */
void UARTDriver::set_device_path(const char *path)
{
    device_path = path;
}

/*
  open the tty
 */
void UARTDriver::begin(uint32_t b)
{
    begin(b, 0, 0);
}

void UARTDriver::begin(uint32_t b, uint16_t rxS, uint16_t txS)
{
    if (!_initialised) {
        if (device_path == NULL && _console) {
            _device = new ConsoleDevice();
        } else {
            if (device_path == NULL) {
                return;
            }

            _device = _parseDevicePath(device_path);

            if (!_device.get()) {
                ::fprintf(stderr, "Argument is not valid. Fallback to console.\n"
                          "Launch with --help to see an example.\n");
                _device = new ConsoleDevice();
            }
        }
    }

    if (!_connected) {
        _connected = _device->open();
        _device->set_blocking(false);
    }
    _initialised = false;

    while (_in_timer) hal.scheduler->delay(1);

    _device->set_speed(b);

    _allocate_buffers(rxS, txS);
}

void UARTDriver::_allocate_buffers(uint16_t rxS, uint16_t txS)
{
    /* we have enough memory to have a larger transmit buffer for
     * all ports. This means we don't get delays while waiting to
     * write GPS config packets
     */

    if (rxS < 8192) {
        rxS = 8192;
    }
    if (txS < 32000) {
        txS = 32000;
    }

    if (_writebuf.set_size(txS) && _readbuf.set_size(rxS)) {
        _initialised = true;
    }
}

void UARTDriver::_deallocate_buffers()
{
    _readbuf.set_size(0);
    _writebuf.set_size(0);
}

/*
    Device path accepts the following syntaxes:
        - /dev/ttyO1
        - tcp:*:1243:wait
        - udp:192.168.2.15:1243
*/
AP_HAL::OwnPtr<SerialDevice> UARTDriver::_parseDevicePath(const char *arg)
{
    struct stat st;

    if (stat(arg, &st) == 0 && S_ISCHR(st.st_mode)) {
        return AP_HAL::OwnPtr<SerialDevice>(new UARTDevice(arg));
#if CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_QFLIGHT
    } else if (strncmp(arg, "qflight:", 8) == 0) {
        return AP_HAL::OwnPtr<SerialDevice>(new QFLIGHTDevice(device_path));
#endif
    } else if (strncmp(arg, "tcp:", 4) != 0 &&
               strncmp(arg, "udp:", 4) != 0 &&
               strncmp(arg, "udpin:", 6)) {
        return nullptr;
    }

    char *devstr = strdup(arg);

    if (devstr == NULL) {
        return nullptr;
    }

    char *saveptr = NULL;
    char *protocol, *ip, *port, *flag;

    protocol = strtok_r(devstr, ":", &saveptr);
    ip = strtok_r(NULL, ":", &saveptr);
    port = strtok_r(NULL, ":", &saveptr);
    flag = strtok_r(NULL, ":", &saveptr);

    if (ip == NULL || port == NULL) {
        free(devstr);
        return nullptr;
    }

    if (_ip) {
        free(_ip);
        _ip = NULL;
    }

    if (_flag) {
        free(_flag);
        _flag = NULL;
    }

    _base_port = (uint16_t) atoi(port);
    _ip = strdup(ip);

    /* Optional flag for TCP */
    if (flag != NULL) {
        _flag = strdup(flag);
    }

    AP_HAL::OwnPtr<SerialDevice> device = nullptr;

    if (strcmp(protocol, "udp") == 0 || strcmp(protocol, "udpin") == 0) {
        bool bcast = (_flag && strcmp(_flag, "bcast") == 0);
        _packetise = true;
        if (strcmp(protocol, "udp") == 0) {
            device = new UDPDevice(_ip, _base_port, bcast, false);
        } else {
            if (bcast) {
                AP_HAL::panic("Can't combine udpin with bcast");
            }
            device = new UDPDevice(_ip, _base_port, false, true);

        }
    } else {
        bool wait = (_flag && strcmp(_flag, "wait") == 0);
        device = new TCPServerDevice(_ip, _base_port, wait);
    }

    free(devstr);
    return device;
}

/*
  shutdown a UART
 */
void UARTDriver::end()
{
    _initialised = false;
    _connected = false;

    while (_in_timer) {
        hal.scheduler->delay(1);
    }

    _device->close();
    _deallocate_buffers();
}


void UARTDriver::flush()
{
    // we are not doing any buffering, so flush is a no-op
}


/*
  return true if the UART is initialised
 */
bool UARTDriver::is_initialized()
{
    return _initialised;
}


/*
  enable or disable blocking writes
 */
void UARTDriver::set_blocking_writes(bool blocking)
{
    _nonblocking_writes = !blocking;
}


/*
  do we have any bytes pending transmission?
 */
bool UARTDriver::tx_pending()
{
    return (_writebuf.available() > 0);
}

/*
  return the number of bytes available to be read
 */
uint32_t UARTDriver::available()
{
    if (!_initialised) {
        return 0;
    }
    return _readbuf.available();
}

/*
  how many bytes are available in the output buffer?
 */
uint32_t UARTDriver::txspace()
{
    if (!_initialised) {
        return 0;
    }
    return _writebuf.space();
}

int16_t UARTDriver::read()
{
    if (!_initialised) {
        return -1;
    }

    uint8_t byte;
    if (!_readbuf.read_byte(&byte)) {
        return -1;
    }

    return byte;
}

/* Linux implementations of Print virtual methods */
size_t UARTDriver::write(uint8_t c)
{
    if (!_initialised) {
        return 0;
    }

    while (_writebuf.space() == 0) {
        if (_nonblocking_writes) {
            return 0;
        }
        hal.scheduler->delay(1);
    }
    return _writebuf.write(&c, 1);
}

/*
  write size bytes to the write buffer
 */
size_t UARTDriver::write(const uint8_t *buffer, size_t size)
{
    if (!_initialised) {
        return 0;
    }
    if (!_nonblocking_writes) {
        /*
          use the per-byte delay loop in write() above for blocking writes
         */
        size_t ret = 0;
        while (size--) {
            if (write(*buffer++) != 1) break;
            ret++;
        }
        return ret;
    }

    return _writebuf.write(buffer, size);
}

/*
  try writing n bytes, handling an unresponsive port
 */
int UARTDriver::_write_fd(const uint8_t *buf, uint16_t n)
{
    /*
      allow for delayed connection. This allows ArduPilot to start
      before a network interface is available.
     */
    if (!_connected) {
        _connected = _device->open();
    }
    if (!_connected) {
        return 0;
    }

    return _device->write(buf, n);
}

/*
  try reading n bytes, handling an unresponsive port
 */
int UARTDriver::_read_fd(uint8_t *buf, uint16_t n)
{
    return _device->read(buf, n);
}


/*
  try to push out one lump of pending bytes
  return true if progress is made
 */
bool UARTDriver::_write_pending_bytes(void)
{
    uint16_t n;

    // write any pending bytes
    uint32_t available_bytes = _writebuf.available();
    n = available_bytes;
    if (_packetise && n > 0 &&
        (_writebuf.peek(0) != MAVLINK_STX_MAVLINK1 &&
         _writebuf.peek(0) != MAVLINK_STX)) {
        /*
          we have a non-mavlink packet at the start of the
          buffer. Look ahead for a MAVLink start byte, up to 256 bytes
          ahead
         */
        uint16_t limit = n>256?256:n;
        uint16_t i;
        for (i=0; i<limit; i++) {
            int16_t b = _writebuf.peek(i);
            if (b == MAVLINK_STX_MAVLINK1 || b == MAVLINK_STX) {
                n = i;
                break;
            }
        }
        // if we didn't find a MAVLink marker then limit the send size to 256
        if (i == limit) {
            n = limit;
        }
    }
    const int16_t b = _writebuf.peek(0);
    if (_packetise && n > 0 && b > 0 &&
        (b == MAVLINK_STX_MAVLINK1 || b == MAVLINK_STX)) {
        uint8_t min_length = (b == MAVLINK_STX_MAVLINK1)?8:12;
        // this looks like a MAVLink packet - try to write on
        // packet boundaries when possible
        if (n < min_length) {
            // we need to wait for more data to arrive
            n = 0;
        } else {
            // the length of the packet is the 2nd byte, and mavlink
            // packets have a 6 byte header plus 2 byte checksum,
            // giving len+8 bytes
            int16_t len = _writebuf.peek(1);
            if (b == MAVLINK_STX) {
                // check for signed packet with extra 13 bytes
                int16_t incompat_flags = _writebuf.peek(2);
                if (incompat_flags & MAVLINK_IFLAG_SIGNED) {
                    min_length += MAVLINK_SIGNATURE_BLOCK_LEN;
                }
            }
            if (n < len+min_length) {
                // we don't have a full packet yet
                n = 0;
            } else if (n > len+min_length) {
                // send just 1 packet at a time (so MAVLink packets
                // are aligned on UDP boundaries)
                n = len+min_length;
            }
        }
    }

    if (n > 0) {
        int ret;

        if (_packetise) {
            // keep as a single UDP packet
            uint8_t tmpbuf[n];
            _writebuf.peekbytes(tmpbuf, n);
            ret = _write_fd(tmpbuf, n);
            if (ret > 0)
                _writebuf.advance(ret);
        } else {
            ByteBuffer::IoVec vec[2];
            const auto n_vec = _writebuf.peekiovec(vec, n);
            for (int i = 0; i < n_vec; i++) {
                ret = _write_fd(vec[i].data, (uint16_t)vec[i].len);
                if (ret < 0) {
                    break;
                }
                _writebuf.advance(ret);

                /* We wrote less than we asked for, stop */
                if ((unsigned)ret != vec[i].len) {
                    break;
                }
            }
        }
    }

    return _writebuf.available() != available_bytes;
}

/*
  push any pending bytes to/from the serial port. This is called at
  1kHz in the timer thread. Doing it this way reduces the system call
  overhead in the main task enormously.
 */
void UARTDriver::_timer_tick(void)
{
    if (!_initialised) return;

    _in_timer = true;

    uint8_t num_send = 10;
    while (num_send != 0 && _write_pending_bytes()) {
        num_send--;
    }

    // try to fill the read buffer
    int ret;
    ByteBuffer::IoVec vec[2];

    const auto n_vec = _readbuf.reserve(vec, _readbuf.space());
    for (int i = 0; i < n_vec; i++) {
        ret = _read_fd(vec[i].data, vec[i].len);
        if (ret < 0) {
            break;
        }
        _readbuf.commit((unsigned)ret);

        /* stop reading as we read less than we asked for */
        if ((unsigned)ret < vec[i].len) {
            break;
        }
    }

    _in_timer = false;
}
