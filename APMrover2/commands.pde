// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/* Functions in this file:
	void set_next_WP(const AP_Mission::Mission_Command& cmd)
	void set_guided_WP(void)
	void init_home()
	void restart_nav()
************************************************************ 
*/


/*
This function stores waypoint commands
It looks to see what the next command type is and finds the last command.
*/
static void set_next_WP(const AP_Mission::Mission_Command& cmd)
{
	// copy the current WP into the OldWP slot
	// ---------------------------------------
	prev_WP = next_WP;

	// Load the next_WP slot
	// ---------------------
	next_WP = cmd;

    // are we already past the waypoint? This happens when we jump
    // waypoints, and it can cause us to skip a waypoint. If we are
    // past the waypoint when we start on a leg, then use the current
    // location as the previous waypoint, to prevent immediately
    // considering the waypoint complete
    if (location_passed_point(current_loc, prev_WP.content.location, next_WP.content.location)) {
        gcs_send_text_P(SEVERITY_LOW, PSTR("Resetting prev_WP"));
        prev_WP.content.location = current_loc;
    }

	// this is handy for the groundstation
	wp_totalDistance 	= get_distance(current_loc, next_WP.content.location);
	wp_distance 		= wp_totalDistance;
}

static void set_guided_WP(void)
{
	// copy the current location into the OldWP slot
	// ---------------------------------------
	prev_WP.content.location = current_loc;

	// Load the next_WP slot
	// ---------------------
	next_WP.content.location = guided_WP;

	// this is handy for the groundstation
	wp_totalDistance 	= get_distance(current_loc, next_WP.content.location);
	wp_distance 		= wp_totalDistance;
}

// run this at setup on the ground
// -------------------------------
void init_home()
{
    if (!have_position) {
        // we need position information
        return;
    }

	gcs_send_text_P(SEVERITY_LOW, PSTR("init home"));

    ahrs.set_home(g_gps->latitude, g_gps->longitude, g_gps->altitude_cm);
	home_is_set = true;

	// Save Home to EEPROM
	mission.write_home_to_storage();

	// Save prev loc
	// -------------
	next_WP.content.location = prev_WP.content.location = home;

	// Load home for a default guided_WP
	// -------------
	guided_WP = home;
}

static void restart_nav()
{  
    g.pidSpeedThrottle.reset_I();
    prev_WP.content.location = current_loc;
    mission.resume();
}
