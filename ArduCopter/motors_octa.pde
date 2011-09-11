/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#if FRAME_CONFIG ==	OCTA_FRAME

static void output_motors_armed()
{
	int roll_out, pitch_out;
	int out_min = g.rc_3.radio_min;
	int out_max = g.rc_3.radio_max;

	// Throttle is 0 to 1000 only
	g.rc_3.servo_out 	= constrain(g.rc_3.servo_out, 0, 1000);

	if(g.rc_3.servo_out > 0)
		out_min = g.rc_3.radio_min + MINIMUM_THROTTLE;

	g.rc_1.calc_pwm();
	g.rc_2.calc_pwm();
	g.rc_3.calc_pwm();
	g.rc_4.calc_pwm();

	if(g.frame_orientation == X_FRAME){
		roll_out 	 	= (float)g.rc_1.pwm_out * 0.4;
		pitch_out 	 	= (float)g.rc_2.pwm_out * 0.4;

		//Front side
		motor_out[CH_1]		= g.rc_3.radio_out + g.rc_2.pwm_out - roll_out;	 // CW	 FRONT RIGHT
		motor_out[CH_7] 	= g.rc_3.radio_out + g.rc_2.pwm_out + roll_out;	 // CCW	 FRONT LEFT

		//Back side
		motor_out[CH_2]		= g.rc_3.radio_out - g.rc_2.pwm_out + roll_out;	 // CW	 BACK LEFT
		motor_out[CH_4]		= g.rc_3.radio_out - g.rc_2.pwm_out - roll_out;	 // CCW  BACK RIGHT

		//Left side
		motor_out[CH_10] 	= g.rc_3.radio_out + g.rc_1.pwm_out + pitch_out; // CW	 LEFT FRONT
		motor_out[CH_8] 	= g.rc_3.radio_out + g.rc_1.pwm_out - pitch_out; // CCW	 LEFT BACK

		//Right side
		motor_out[CH_11] 	= g.rc_3.radio_out - g.rc_1.pwm_out - pitch_out; // CW	 RIGHT BACK
		motor_out[CH_3]		= g.rc_3.radio_out - g.rc_1.pwm_out + pitch_out; // CCW	 RIGHT FRONT

	}else if(g.frame_orientation == PLUS_FRAME){
		roll_out 		= (float)g.rc_1.pwm_out * 0.71;
		pitch_out 	 	= (float)g.rc_2.pwm_out * 0.71;

		//Front side
		motor_out[CH_1]		= g.rc_3.radio_out + g.rc_2.pwm_out;		// CW	FRONT
		motor_out[CH_3] 	= g.rc_3.radio_out - roll_out + pitch_out;	// CCW	FRONT RIGHT
		motor_out[CH_7] 	= g.rc_3.radio_out + roll_out + pitch_out;	// CCW	FRONT LEFT

		//Left side
		motor_out[CH_10] 	= g.rc_3.radio_out + g.rc_1.pwm_out;		// CW	LEFT

		//Right side
		motor_out[CH_11] 	= g.rc_3.radio_out - g.rc_1.pwm_out;		// CW	RIGHT

		//Back side
		motor_out[CH_2]		= g.rc_3.radio_out - g.rc_2.pwm_out;		// CW	BACK
		motor_out[CH_4]		= g.rc_3.radio_out - roll_out - pitch_out;	// CCW 	BACK RIGHT
		motor_out[CH_8]		= g.rc_3.radio_out + roll_out - pitch_out;	// CCW	BACK LEFT

	}else if(g.frame_orientation == V_FRAME){

		int roll_out2, pitch_out2;
		int roll_out3, pitch_out3;
		int roll_out4, pitch_out4;

		roll_out 	 	= g.rc_1.pwm_out;
		pitch_out 	 	= g.rc_2.pwm_out;
		roll_out2 	 	= (float)g.rc_1.pwm_out * 0.833;
		pitch_out2 	 	= (float)g.rc_2.pwm_out * 0.34;
		roll_out3 	 	= (float)g.rc_1.pwm_out * 0.666;
		pitch_out3 	 	= (float)g.rc_2.pwm_out * 0.32;
		roll_out4 	 	= g.rc_1.pwm_out / 2;
		pitch_out4 	 	= (float)g.rc_2.pwm_out * 0.98;

		//Front side
		motor_out[CH_10]	= g.rc_3.radio_out + g.rc_2.pwm_out - roll_out;		// CW  FRONT RIGHT
		motor_out[CH_7] 	= g.rc_3.radio_out + g.rc_2.pwm_out + roll_out;		// CCW FRONT LEFT

		//Left side
		motor_out[CH_1] 	= g.rc_3.radio_out + g.rc_1.pwm_out + pitch_out2; 	// CW  LEFT FRONT
		motor_out[CH_3] 	= g.rc_3.radio_out + g.rc_1.pwm_out - pitch_out3;	// CCW LEFT BACK

		//Right side
		motor_out[CH_2] 	= g.rc_3.radio_out - g.rc_1.pwm_out - pitch_out3;	// CW  RIGHT BACK
		motor_out[CH_8]		= g.rc_3.radio_out - g.rc_1.pwm_out + pitch_out2;	// CCW RIGHT FRONT

		//Back side
		motor_out[CH_11]	= g.rc_3.radio_out - g.rc_2.pwm_out + roll_out4;	// CW  BACK LEFT
		motor_out[CH_4]		= g.rc_3.radio_out - g.rc_2.pwm_out - roll_out4;	// CCW BACK RIGHT

	}

	// Yaw
	motor_out[CH_3]		+= g.rc_4.pwm_out;	// CCW
	motor_out[CH_4]		+= g.rc_4.pwm_out;	// CCW
	motor_out[CH_7] 	+= g.rc_4.pwm_out;	// CCW
	motor_out[CH_8] 	+= g.rc_4.pwm_out;	// CCW

	motor_out[CH_1]		-= g.rc_4.pwm_out;	// CW
	motor_out[CH_2]		-= g.rc_4.pwm_out;	// CW
	motor_out[CH_10]	-= g.rc_4.pwm_out;	// CW
	motor_out[CH_11]	-= g.rc_4.pwm_out;	// CW


	// TODO add stability patch
	motor_out[CH_1]		= min(motor_out[CH_1], 	out_max);
	motor_out[CH_2]		= min(motor_out[CH_2], 	out_max);
	motor_out[CH_3]		= min(motor_out[CH_3], 	out_max);
	motor_out[CH_4]		= min(motor_out[CH_4], 	out_max);
	motor_out[CH_7]		= min(motor_out[CH_7],  out_max);
	motor_out[CH_8]		= min(motor_out[CH_8],  out_max);
	motor_out[CH_10]	= min(motor_out[CH_10], out_max);
	motor_out[CH_11] 	= min(motor_out[CH_11], out_max);


	// limit output so motors don't stop
	motor_out[CH_1]		= max(motor_out[CH_1], 	out_min);
	motor_out[CH_2]		= max(motor_out[CH_2], 	out_min);
	motor_out[CH_3]		= max(motor_out[CH_3], 	out_min);
	motor_out[CH_4] 	= max(motor_out[CH_4], 	out_min);
	motor_out[CH_7]		= max(motor_out[CH_7], 	out_min);
	motor_out[CH_8] 	= max(motor_out[CH_8], 	out_min);
	motor_out[CH_10]	= max(motor_out[CH_10], out_min);
	motor_out[CH_11] 	= max(motor_out[CH_11], out_min);


	#if CUT_MOTORS == ENABLED
	// if we are not sending a throttle output, we cut the motors
	if(g.rc_3.servo_out == 0){
		motor_out[CH_1]		= g.rc_3.radio_min;
		motor_out[CH_2]		= g.rc_3.radio_min;
		motor_out[CH_3]		= g.rc_3.radio_min;
		motor_out[CH_4] 	= g.rc_3.radio_min;
		motor_out[CH_7] 	= g.rc_3.radio_min;
		motor_out[CH_8] 	= g.rc_3.radio_min;
		motor_out[CH_10] 	= g.rc_3.radio_min;
		motor_out[CH_11] 	= g.rc_3.radio_min;
	}
	#endif

	APM_RC.OutputCh(CH_1, motor_out[CH_1]);
	APM_RC.OutputCh(CH_2, motor_out[CH_2]);
	APM_RC.OutputCh(CH_3, motor_out[CH_3]);
	APM_RC.OutputCh(CH_4, motor_out[CH_4]);
	APM_RC.OutputCh(CH_7, motor_out[CH_7]);
	APM_RC.OutputCh(CH_8, motor_out[CH_8]);
	APM_RC.OutputCh(CH_10, motor_out[CH_10]);
	APM_RC.OutputCh(CH_11, motor_out[CH_11]);

	// InstantPWM
	APM_RC.Force_Out0_Out1();
	APM_RC.Force_Out6_Out7();
	APM_RC.Force_Out2_Out3();
}

static void output_motors_disarmed()
{
	if(g.rc_3.control_in > 0){
		// we have pushed up the throttle
		// remove safety
		motor_auto_armed = true;
	}

	// fill the motor_out[] array for HIL use
	for (unsigned char i = 0; i < 11; i++) {
		motor_out[i] = g.rc_3.radio_min;
	}

	// Send commands to motors
	APM_RC.OutputCh(CH_1, g.rc_3.radio_min);
	APM_RC.OutputCh(CH_2, g.rc_3.radio_min);
	APM_RC.OutputCh(CH_7, g.rc_3.radio_min);
	APM_RC.OutputCh(CH_8, g.rc_3.radio_min);
	APM_RC.OutputCh(CH_11, g.rc_3.radio_min);

	APM_RC.OutputCh(CH_3, g.rc_3.radio_min);
	APM_RC.OutputCh(CH_4, g.rc_3.radio_min);
	APM_RC.OutputCh(CH_10, g.rc_3.radio_min);
}

static void output_motor_test()
{
	APM_RC.OutputCh(CH_7, g.rc_3.radio_min);
	APM_RC.OutputCh(CH_1, g.rc_3.radio_min + 100);
	delay(1000);

	APM_RC.OutputCh(CH_1, g.rc_3.radio_min);
	APM_RC.OutputCh(CH_3, g.rc_3.radio_min + 100);
	delay(1000);

	APM_RC.OutputCh(CH_3, g.rc_3.radio_min);
	APM_RC.OutputCh(CH_11, g.rc_3.radio_min + 100);
	delay(1000);

	APM_RC.OutputCh(CH_11, g.rc_3.radio_min);
	APM_RC.OutputCh(CH_4, g.rc_3.radio_min + 100);
	delay(1000);

	APM_RC.OutputCh(CH_4, g.rc_3.radio_min);
	APM_RC.OutputCh(CH_2, g.rc_3.radio_min + 100);
	delay(1000);

	APM_RC.OutputCh(CH_2, g.rc_3.radio_min);
	APM_RC.OutputCh(CH_8, g.rc_3.radio_min + 100);
	delay(1000);

	APM_RC.OutputCh(CH_8, g.rc_3.radio_min);
	APM_RC.OutputCh(CH_10, g.rc_3.radio_min + 100);
	delay(1000);

	APM_RC.OutputCh(CH_10, g.rc_3.radio_min);
	APM_RC.OutputCh(CH_7, g.rc_3.radio_min + 100);
	delay(1000);
}

#endif

