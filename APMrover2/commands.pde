// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/* Functions in this file:
	void init_commands()
	struct Location get_cmd_with_index(int i)
	void set_cmd_with_index(struct Location temp, int i)
	void increment_cmd_index()
	void decrement_cmd_index()
	long read_alt_to_hold()
	void set_next_WP(struct Location *wp)
	void set_guided_WP(void)
	void init_home()
	void restart_nav()
************************************************************ 
*/

static void init_commands()
{
    g.command_index.set_and_save(0);
	nav_command_ID	= NO_COMMAND;
	non_nav_command_ID	= NO_COMMAND;
	next_nav_command.id 	= CMD_BLANK;
}

// Getters
// -------
static struct Location get_cmd_with_index(int i)
{
	struct Location temp;
	long mem;

	// Find out proper location in memory by using the start_byte position + the index
	// --------------------------------------------------------------------------------
	if (i > g.command_total) {
		memset(&temp, 0, sizeof(temp));
		temp.id = CMD_BLANK;
	}else{
		// read WP position
		mem = (WP_START_BYTE) + (i * WP_SIZE);
		temp.id = eeprom_read_byte((uint8_t*)mem);

		mem++;
		temp.options = eeprom_read_byte((uint8_t*)mem);

		mem++;
		temp.p1 = eeprom_read_byte((uint8_t*)mem);

		mem++;
		temp.alt = (long)eeprom_read_dword((uint32_t*)mem);

		mem += 4;
		temp.lat = (long)eeprom_read_dword((uint32_t*)mem);

		mem += 4;
		temp.lng = (long)eeprom_read_dword((uint32_t*)mem);
	}

	// Add on home altitude if we are a nav command (or other command with altitude) and stored alt is relative
	if((temp.id < MAV_CMD_NAV_LAST || temp.id == MAV_CMD_CONDITION_CHANGE_ALT) && temp.options & MASK_OPTIONS_RELATIVE_ALT){
		temp.alt += home.alt;
	}

	return temp;
}

// Setters
// -------
static void set_cmd_with_index(struct Location temp, int i)
{
	i = constrain(i, 0, g.command_total.get());
	intptr_t mem = WP_START_BYTE + (i * WP_SIZE);

	// Set altitude options bitmask
	// XXX What is this trying to do?
	if (temp.options & MASK_OPTIONS_RELATIVE_ALT && i != 0){
		temp.options = MASK_OPTIONS_RELATIVE_ALT;
	} else {
		temp.options = 0;
	}

	eeprom_write_byte((uint8_t *)	mem, temp.id);

        mem++;
	eeprom_write_byte((uint8_t *)	mem, temp.options);

	mem++;
	eeprom_write_byte((uint8_t *)	mem, temp.p1);

	mem++;
	eeprom_write_dword((uint32_t *)	mem, temp.alt);

	mem += 4;
	eeprom_write_dword((uint32_t *)	mem, temp.lat);

	mem += 4;
	eeprom_write_dword((uint32_t *)	mem, temp.lng);
}

/*
This function stores waypoint commands
It looks to see what the next command type is and finds the last command.
*/
static void set_next_WP(struct Location *wp)
{
	// copy the current WP into the OldWP slot
	// ---------------------------------------
	prev_WP = next_WP;

	// Load the next_WP slot
	// ---------------------
	next_WP = *wp;

    // are we already past the waypoint? This happens when we jump
    // waypoints, and it can cause us to skip a waypoint. If we are
    // past the waypoint when we start on a leg, then use the current
    // location as the previous waypoint, to prevent immediately
    // considering the waypoint complete
    if (location_passed_point(current_loc, prev_WP, next_WP)) {
        gcs_send_text_P(SEVERITY_LOW, PSTR("Resetting prev_WP"));
        prev_WP = current_loc;
    }

	// this is handy for the groundstation
	wp_totalDistance 	= get_distance(&current_loc, &next_WP);
	wp_distance 		= wp_totalDistance;
	target_bearing 		= get_bearing_cd(&current_loc, &next_WP);
	nav_bearing 		= target_bearing;

	// set a new crosstrack bearing
	// ----------------------------
	reset_crosstrack();
}

static void set_guided_WP(void)
{
	// copy the current location into the OldWP slot
	// ---------------------------------------
	prev_WP = current_loc;

	// Load the next_WP slot
	// ---------------------
	next_WP = guided_WP;

	// this is handy for the groundstation
	wp_totalDistance 	= get_distance(&current_loc, &next_WP);
	wp_distance 		= wp_totalDistance;
	target_bearing 		= get_bearing_cd(&current_loc, &next_WP);

	// set a new crosstrack bearing
	// ----------------------------
	reset_crosstrack();
}

// run this at setup on the ground
// -------------------------------
void init_home()
{
	gcs_send_text_P(SEVERITY_LOW, PSTR("init home"));

	// block until we get a good fix
	// -----------------------------
	while (!g_gps->new_data || !g_gps->fix) {
		g_gps->update();
	}

	home.id 	= MAV_CMD_NAV_WAYPOINT;
#if HIL_MODE != HIL_MODE_ATTITUDE

	home.lng 	= g_gps->longitude;				// Lon * 10**7
	home.lat 	= g_gps->latitude;				// Lat * 10**7
        gps_base_alt    = max(g_gps->altitude, 0);
        home.alt        = g_gps->altitude;;
  					// Home is always 0
#else
//	struct Location temp = get_cmd_with_index(0);    // JLN update - for HIL test only get the home param stored in the FPL
//        if (temp.alt > 0) {
//        	home.lng 	= temp.lng;			 // Lon * 10**7
//        	home.lat 	= temp.lat;			 // Lat * 10**7
//        } else {
        	home.lng 	= g_gps->longitude;		 // Lon * 10**7
        	home.lat 	= g_gps->latitude;		 // Lat * 10**7       
//        }
        
         gps_base_alt    = g_gps->altitude;;             // get the stored home altitude as the base ref for AGL calculation.
         home.alt        = g_gps->altitude;;

#endif
	home_is_set = true;

        //gcs_send_text_fmt(PSTR("gps alt: %lu"), (unsigned long)home.alt);

	// Save Home to EEPROM - Command 0
	// -------------------
	set_cmd_with_index(home, 0);

	// Save prev loc
	// -------------
	next_WP = prev_WP = home;

	// Load home for a default guided_WP
	// -------------
	guided_WP = home;
}

static void restart_nav()
{  
  reset_I();
  prev_WP = current_loc;
  nav_command_ID = NO_COMMAND;
  nav_command_index = 0;
  process_next_command();
}
