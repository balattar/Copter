
enum log_messages {
    // plane specific messages
    LOG_PLANE_NTUN_MSG     = 2,
    LOG_PLANE_ATTITUDE_MSG = 10,
    LOG_PLANE_COMPASS_MSG  = 12,

    // copter specific messages
    LOG_COPTER_ATTITUDE_MSG = 1,
    LOG_COPTER_COMPASS_MSG  = 15,
    LOG_COPTER_CONTROL_TUNING_MSG = 4
};


class LogReader 
{
public:
    LogReader(AP_InertialSensor &_ins, AP_Baro_HIL &_baro, AP_Compass_HIL &_compass, GPS *&_gps);
    bool open_log(const char *logfile);
    bool update(uint8_t &type);
    bool wait_type(uint8_t type);
    const Vector3f &get_attitude(void) const { return attitude; }
    const Vector3f &get_sim_attitude(void) const { return sim_attitude; }

    enum vehicle_type { VEHICLE_UNKNOWN, VEHICLE_COPTER, VEHICLE_PLANE, VEHICLE_ROVER };

    vehicle_type vehicle;

private:
    int fd;
    AP_InertialSensor &ins;
    AP_Baro_HIL &baro;
    AP_Compass_HIL &compass;
    GPS *&gps;

    uint32_t ground_alt_cm;

    uint8_t num_formats;
    struct log_Format formats[100];

    Vector3f attitude;
    Vector3f sim_attitude;

    void wait_timestamp(uint32_t timestamp);

    void process_plane(uint8_t type, uint8_t *data, uint16_t length);
    void process_copter(uint8_t type, uint8_t *data, uint16_t length);
};
