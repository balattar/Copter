/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
#include <AP_HAL.h>
#include <AC_WPNav.h>

extern const AP_HAL::HAL& hal;

const AP_Param::GroupInfo AC_WPNav::var_info[] PROGMEM = {
    // index 0 was used for the old orientation matrix

    // @Param: SPEED
    // @DisplayName: Waypoint Speed Target
    // @Description: Defines the speed in cm/s which the aircraft will attempt to maintain during a WP mission
    // @Units: Centimeters/Second
    // @Range: 0 1000
    // @Increment: 100
    // @User: Standard
    AP_GROUPINFO("SPEED",   0, AC_WPNav, _speed_cms, WPNAV_WP_SPEED),

    // @Param: RADIUS
    // @DisplayName: Waypoint Radius
    // @Description: Defines the distance from a waypoint, that when crossed indicates the wp has been hit.
    // @Units: Centimeters
    // @Range: 100 1000
    // @Increment: 1
    // @User: Standard
    AP_GROUPINFO("RADIUS",      1, AC_WPNav, _wp_radius_cm, WPNAV_WP_RADIUS),

    AP_GROUPEND
};

// Default constructor.
// Note that the Vector/Matrix constructors already implicitly zero
// their values.
//
AC_WPNav::AC_WPNav(AP_InertialNav* inav, APM_PI* pid_pos_lat, APM_PI* pid_pos_lon, AC_PID* pid_rate_lat, AC_PID* pid_rate_lon) :
    _inav(inav),
    _pid_pos_lat(pid_pos_lat),
    _pid_pos_lon(pid_pos_lon),
    _pid_rate_lat(pid_rate_lat),
    _pid_rate_lon(pid_rate_lon),
    _speedz_cms(MAX_CLIMB_VELOCITY),
    _lean_angle_max(MAX_LEAN_ANGLE)
{
    AP_Param::setup_object_defaults(this, var_info);
}

///
/// simple loiter controller
///

/// set_loiter_target - set initial loiter target based on current position and velocity
void AC_WPNav::set_loiter_target(const Vector3f& position, const Vector3f& velocity)
{
    float linear_distance;      // half the distace we swap between linear and sqrt and the distace we offset sqrt.
    float linear_velocity;      // the velocity we swap between linear and sqrt.
    float vel_total;
    float target_dist;

    // avoid divide by zero
    if( _pid_pos_lat->kP() <= 0.1 ) {
        set_loiter_target(position);
        return;
    }

    // calculate point at which velocity switches from linear to sqrt
    linear_velocity = MAX_LOITER_POS_ACCEL/_pid_pos_lat->kP();

    // calculate total current velocity
    vel_total = safe_sqrt(velocity.x*velocity.x + velocity.y*velocity.y);

    // calculate distance within which we can stop
    if (vel_total < linear_velocity) {
        target_dist = vel_total/_pid_pos_lat->kP();
    } else {
        linear_distance = MAX_LOITER_POS_ACCEL/(2*_pid_pos_lat->kP()*_pid_pos_lat->kP());
        target_dist = linear_distance + (vel_total*vel_total)/(2*MAX_LOITER_POS_ACCEL);
    }
    target_dist = constrain(target_dist, 0, MAX_LOITER_OVERSHOOT);

    _target.x = position.x + (target_dist * velocity.x / vel_total);
    _target.y = position.y + (target_dist * velocity.y / vel_total);
}

/// move_loiter_target - move loiter target by velocity provided in front/right directions in cm/s
void AC_WPNav::move_loiter_target(float control_roll, float control_pitch, float dt)
{
    // convert pilot input to desired velocity in cm/s
    _pilot_vel_forward_cms = -control_pitch * MAX_LOITER_POS_VELOCITY / 4500.0f;
    _pilot_vel_right_cms = control_roll * MAX_LOITER_POS_VELOCITY / 4500.0f;
}

/// translate_loiter_target_movements - consumes adjustments created by move_loiter_target
void AC_WPNav::translate_loiter_target_movements(float nav_dt)
{
    Vector2f target_vel_adj;    // make 2d vector?
    float vel_delta_total;
    float vel_max;
    float vel_total;

    // range check nav_dt
    if( nav_dt < 0 ) {
        return;
    }

    // rotate pilot input to lat/lon frame
    target_vel_adj.x = (_pilot_vel_forward_cms*_cos_yaw - _pilot_vel_right_cms*_sin_yaw) - _target_vel.x;
    target_vel_adj.y = (_pilot_vel_forward_cms*_sin_yaw + _pilot_vel_right_cms*_cos_yaw) - _target_vel.y;

    // constrain the velocity vector and scale if necessary
    vel_delta_total = safe_sqrt(target_vel_adj.x*target_vel_adj.x + target_vel_adj.y*target_vel_adj.y);
    vel_max = MAX_LOITER_POS_ACCEL*nav_dt;
    if( vel_delta_total >  vel_max) {
        target_vel_adj.x = vel_max * target_vel_adj.x/vel_delta_total;
        target_vel_adj.y = vel_max * target_vel_adj.y/vel_delta_total;
    }

    // add desired change in velocity to current target velocity
    _target_vel.x += target_vel_adj.x;
    _target_vel.y += target_vel_adj.y;

    // constrain the velocity vector and scale if necessary
    vel_total = safe_sqrt(_target_vel.x*_target_vel.x + _target_vel.y*_target_vel.y);
    if( vel_total > MAX_LOITER_POS_VELOCITY ) {
        _target_vel.x = MAX_LOITER_POS_VELOCITY * _target_vel.x/vel_total;
        _target_vel.y = MAX_LOITER_POS_VELOCITY * _target_vel.y/vel_total;
    }

    // update target position
    _target.x += _target_vel.x * nav_dt;
    _target.y += _target_vel.y * nav_dt;

    // constrain target position to within reasonable distance of current location
    Vector3f curr_pos = _inav->get_position();
    Vector3f distance_err = _target - curr_pos;
    float distance = safe_sqrt(distance_err.x*distance_err.x + distance_err.y*distance_err.y);
    if( distance > MAX_LOITER_OVERSHOOT ) {
        _target.x = curr_pos.x + MAX_LOITER_OVERSHOOT * distance_err.x/distance;
        _target.y = curr_pos.y + MAX_LOITER_OVERSHOOT * distance_err.y/distance;
    }
}

/// get_distance_to_target - get horizontal distance to loiter target in cm
float AC_WPNav::get_distance_to_target()
{
    return _distance_to_target;
}

/// get_bearing_to_target - get bearing to loiter target in centi-degrees
int32_t AC_WPNav::get_bearing_to_target()
{
    return get_bearing_cd(_inav->get_position(), _target);
}

/// update_loiter - run the loiter controller - should be called at 10hz
void AC_WPNav::update_loiter()
{
    uint32_t now = hal.scheduler->millis();
    float dt = (now - _last_update) / 1000.0f;
    _last_update = now;

    // catch if we've just been started
    if( dt >= 1.0 ) {
        dt = 0.0;
        reset_I();
        _target_vel.x = 0;
        _target_vel.y = 0;
    }

    // translate any adjustments from pilot to loiter target
    translate_loiter_target_movements(dt);

    // run loiter position controller
    get_loiter_pos_lat_lon(_target.x, _target.y, dt);
}

///
/// waypoint navigation
///

/// set_destination - set destination using cm from home
void AC_WPNav::set_destination(const Vector3f& destination)
{
    // To-Do: use projection of current position & velocity to set origin
    set_origin_and_destination(_inav->get_position(), destination);
}

/// set_origin_and_destination - set origin and destination using lat/lon coordinates
void AC_WPNav::set_origin_and_destination(const Vector3f& origin, const Vector3f& destination)
{
    _origin = origin;
    _destination = destination;

    _vert_track_scale = WPINAV_MAX_POS_ERROR / WPINAV_MAX_ALT_ERROR;
    Vector3f pos_delta = _destination - _origin;
    pos_delta.z = pos_delta.z * _vert_track_scale;
    _track_length = pos_delta.length();
    _pos_delta_unit = pos_delta/_track_length;

    _track_desired = 0;
    _reached_destination = false;
}

/// advance_target_along_track - move target location along track from origin to destination
void AC_WPNav::advance_target_along_track(float velocity_cms, float dt)
{
    float track_covered;
    float track_error;
    float track_desired_max;
    float track_desired_temp = _track_desired;
    float track_extra_max;
    float curr_delta_length;

    // get current location
    Vector3f curr_pos = _inav->get_position();
    Vector3f curr_delta = curr_pos - _origin;
    curr_delta.z = curr_delta.z * _vert_track_scale;
    curr_delta_length = curr_delta.length();

    track_covered = curr_delta.x * _pos_delta_unit.x + curr_delta.y * _pos_delta_unit.y + curr_delta.z * _pos_delta_unit.z;
    track_error = safe_sqrt(curr_delta_length*curr_delta_length - track_covered*track_covered);

    track_extra_max = safe_sqrt(WPINAV_MAX_POS_ERROR*WPINAV_MAX_POS_ERROR - track_error*track_error);

    // we could save a sqrt by doing the following and not assigning track_error
    // track_extra_max = safe_sqrt(WPINAV_MAX_POS_ERROR*WPINAV_MAX_POS_ERROR - (curr_delta_length*curr_delta_length - track_covered*track_covered));

    track_desired_max = track_covered + track_extra_max;

    // advance the current target
    track_desired_temp += velocity_cms * dt;

    // constrain the target from moving too far
    if( track_desired_temp > track_desired_max ) {
        track_desired_temp = track_desired_max;
    }
    // do not let desired point go past the end of the segment
    track_desired_temp = constrain(track_desired_temp, 0, _track_length);
    _track_desired = max(_track_desired, track_desired_temp);

    // recalculate the desired position
    _target.x = _origin.x + _pos_delta_unit.x * _track_desired;
    _target.y = _origin.y + _pos_delta_unit.y * _track_desired;
    _target.z = _origin.z + (_pos_delta_unit.z * _track_desired)/_vert_track_scale;

    // check if we've reached the waypoint
    if( !_reached_destination ) {
        if( _track_desired >= _track_length ) {
            Vector3f dist_to_dest = curr_pos - _destination;
            dist_to_dest.z *=_vert_track_scale;
            if( dist_to_dest.length() <= _wp_radius_cm ) {
                _reached_destination = true;
            }
        }
    }
}

/// get_distance_to_destination - get horizontal distance to destination in cm
float AC_WPNav::get_distance_to_destination()
{
    // get current location
    Vector3f curr = _inav->get_position();
    return pythagorous2(_destination.x-curr.x,_destination.y-curr.y);
}

/// get_bearing_to_destination - get bearing to next waypoint in centi-degrees
int32_t AC_WPNav::get_bearing_to_destination()
{
    return get_bearing_cd(_inav->get_position(), _destination);
}

/// update_wpnav - run the wp controller - should be called at 10hz
void AC_WPNav::update_wpnav()
{
    uint32_t now = hal.scheduler->millis();
    float dt = (now - _last_update) / 1000.0f;
    _last_update = now;

    // catch if we've just been started
    if( dt >= 1.0 ) {
        dt = 0.0;
        reset_I();
    }else{
        // advance the target if necessary
        advance_target_along_track(_speed_cms, dt);
    }

    // run loiter position controller
    get_loiter_pos_lat_lon(_target.x, _target.y, dt);
}

///
/// shared methods
///

// get_loiter_pos_lat_lon - loiter position controller
//     converts desired position provided as distance from home in lat/lon directions to desired velocity
void AC_WPNav::get_loiter_pos_lat_lon(float target_lat_from_home, float target_lon_from_home, float dt)
{
    Vector2f dist_error;
    Vector2f desired_vel;
    Vector3f curr = _inav->get_position();
    float dist_error_total;

    float vel_sqrt;
    float vel_total;

    float linear_distance;      // the distace we swap between linear and sqrt.

    // calculate distance error
    dist_error.x = target_lat_from_home - curr.x;
    dist_error.y = target_lon_from_home - curr.y;

    linear_distance = MAX_LOITER_POS_ACCEL/(2*_pid_pos_lat->kP()*_pid_pos_lat->kP());
    _distance_to_target = linear_distance;      // for reporting purposes

    dist_error_total = safe_sqrt(dist_error.x*dist_error.x + dist_error.y*dist_error.y);
    if( dist_error_total > 2*linear_distance ) {
        vel_sqrt = constrain(safe_sqrt(2*MAX_LOITER_POS_ACCEL*(dist_error_total-linear_distance)),0,1000);
        desired_vel.x = vel_sqrt * dist_error.x/dist_error_total;
        desired_vel.y = vel_sqrt * dist_error.y/dist_error_total;
    }else{
        desired_vel.x = _pid_pos_lat->get_p(dist_error.x);
        desired_vel.y = _pid_pos_lon->get_p(dist_error.y);
    }

    vel_total = safe_sqrt(desired_vel.x*desired_vel.x + desired_vel.y*desired_vel.y);
    if( vel_total > MAX_LOITER_POS_VELOCITY ) {
        desired_vel.x = MAX_LOITER_POS_VELOCITY * desired_vel.x/vel_total;
        desired_vel.y = MAX_LOITER_POS_VELOCITY * desired_vel.y/vel_total;
    }

    get_loiter_vel_lat_lon(desired_vel.x, desired_vel.y, dt);
}

// get_loiter_vel_lat_lon - loiter velocity controller
//    converts desired velocities in lat/lon frame to accelerations in lat/lon frame
void AC_WPNav::get_loiter_vel_lat_lon(float vel_lat, float vel_lon, float dt)
{
    Vector3f vel_curr = _inav->get_velocity();  // current velocity in cm/s
    Vector3f vel_error;                         // The velocity error in cm/s.
    Vector2f desired_accel;                     // the resulting desired acceleration
    float accel_total;                          // total acceleration in cm/s/s

    // reset last velocity if this controller has just been engaged or dt is zero
    if( dt == 0.0 ) {
        desired_accel.x = 0;
        desired_accel.y = 0;
    } else {
        // feed forward desired acceleration calculation
        desired_accel.x = (vel_lat - _vel_last.x)/dt;
        desired_accel.y = (vel_lon - _vel_last.y)/dt;
    }

    // store this iteration's velocities for the next iteration
    _vel_last.x = vel_lat;
    _vel_last.y = vel_lon;

    // calculate velocity error
    vel_error.x = vel_lat - vel_curr.x;
    vel_error.y = vel_lon - vel_curr.y;

    // combine feed foward accel with PID outpu from velocity error
    desired_accel.x += _pid_rate_lat->get_pid(vel_error.x, dt);
    desired_accel.y += _pid_rate_lon->get_pid(vel_error.y, dt);

    // scale desired acceleration if it's beyond acceptable limit
    accel_total = safe_sqrt(desired_accel.x*desired_accel.x + desired_accel.y*desired_accel.y);
    if( accel_total > MAX_LOITER_VEL_ACCEL ) {
        desired_accel.x = MAX_LOITER_VEL_ACCEL * desired_accel.x/accel_total;
        desired_accel.y = MAX_LOITER_VEL_ACCEL * desired_accel.y/accel_total;
    }

    // call accel based controller with desired acceleration
    get_loiter_accel_lat_lon(desired_accel.x, desired_accel.y);
}

// get_loiter_accel_lat_lon - loiter acceration controller
//    converts desired accelerations provided in lat/lon frame to roll/pitch angles
void AC_WPNav::get_loiter_accel_lat_lon(float accel_lat, float accel_lon)
{
    float z_accel_meas = -GRAVITY_MSS * 100;    // gravity in cm/s/s
    float accel_forward;
    float accel_right;

    // To-Do: add 1hz filter to accel_lat, accel_lon

    // rotate accelerations into body forward-right frame
    accel_forward = accel_lat*_cos_yaw + accel_lon*_sin_yaw;
    accel_right = -accel_lat*_sin_yaw + accel_lon*_cos_yaw;

    // update angle targets that will be passed to stabilize controller
    _desired_roll = constrain((accel_right/(-z_accel_meas))*(18000/M_PI), -_lean_angle_max, _lean_angle_max);
    _desired_pitch = constrain((-accel_forward/(-z_accel_meas*_cos_roll))*(18000/M_PI), -_lean_angle_max, _lean_angle_max);
}

// get_bearing_cd - return bearing in centi-degrees between two positions
// To-Do: move this to math library
float AC_WPNav::get_bearing_cd(const Vector3f origin, const Vector3f destination)
{
    float bearing = 9000 + atan2f(-(destination.x-origin.x), destination.y-origin.y) * 5729.57795f;
    if (bearing < 0) {
        bearing += 36000;
    }
    return bearing;
}

/// reset_I - clears I terms from loiter PID controller
void AC_WPNav::reset_I()
{
    _pid_pos_lon->reset_I();
    _pid_pos_lat->reset_I();
    _pid_rate_lon->reset_I();
    _pid_rate_lat->reset_I();

    // set last velocity to current velocity
    _vel_last = _inav->get_velocity();
}