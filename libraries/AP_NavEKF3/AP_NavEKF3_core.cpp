/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <AP_HAL/AP_HAL.h>

#if HAL_CPU_CLASS >= HAL_CPU_CLASS_150

#include "AP_NavEKF3.h"
#include "AP_NavEKF3_core.h"
#include <AP_AHRS/AP_AHRS.h>
#include <AP_Vehicle/AP_Vehicle.h>

#include <stdio.h>

extern const AP_HAL::HAL& hal;

// constructor
NavEKF3_core::NavEKF3_core(void) :
    stateStruct(*reinterpret_cast<struct state_elements *>(&statesArray)),

    _perf_UpdateFilter(hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_UpdateFilter")),
    _perf_CovariancePrediction(hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_CovariancePrediction")),
    _perf_FuseVelPosNED(hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_FuseVelPosNED")),
    _perf_FuseMagnetometer(hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_FuseMagnetometer")),
    _perf_FuseAirspeed(hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_FuseAirspeed")),
    _perf_FuseSideslip(hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_FuseSideslip")),
    _perf_TerrainOffset(hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_TerrainOffset")),
    _perf_FuseOptFlow(hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_FuseOptFlow"))
{
    _perf_test[0] = hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_Test0");
    _perf_test[1] = hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_Test1");
    _perf_test[2] = hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_Test2");
    _perf_test[3] = hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_Test3");
    _perf_test[4] = hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_Test4");
    _perf_test[5] = hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_Test5");
    _perf_test[6] = hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_Test6");
    _perf_test[7] = hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_Test7");
    _perf_test[8] = hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_Test8");
    _perf_test[9] = hal.util->perf_alloc(AP_HAL::Util::PC_ELAPSED, "EK2_Test9");
}

// setup this core backend
bool NavEKF3_core::setup_core(NavEKF3 *_frontend, uint8_t _imu_index, uint8_t _core_index)
{
    frontend = _frontend;
    imu_index = _imu_index;
    core_index = _core_index;
    _ahrs = frontend->_ahrs;

    /*
      the imu_buffer_length needs to cope with a 260ms delay at a
      maximum fusion rate of 100Hz. Non-imu data coming in at faster
      than 100Hz is downsampled. For 50Hz main loop rate we need a
      shorter buffer.
     */
    if (_ahrs->get_ins().get_sample_rate() < 100) {
        imu_buffer_length = 13;
    } else {
        // maximum 260 msec delay at 100 Hz fusion rate
        imu_buffer_length = 26;
    }
    if(!storedGPS.init(OBS_BUFFER_LENGTH)) {
        return false;
    }
    if(!storedMag.init(OBS_BUFFER_LENGTH)) {
        return false;
    }
    if(!storedBaro.init(OBS_BUFFER_LENGTH)) {
        return false;
    }
    if(!storedTAS.init(OBS_BUFFER_LENGTH)) {
        return false;
    }
    if(!storedOF.init(OBS_BUFFER_LENGTH)) {
        return false;
    }
    // Note: the use of dual range finders potentially doubles the amount of to be stored
    if(!storedRange.init(2*OBS_BUFFER_LENGTH)) {
        return false;
    }
    // Note: range beacon data is read one beacon at a time and can arrive at a high rate
    if(!storedRangeBeacon.init(imu_buffer_length)) {
        return false;
    }
    if(!storedIMU.init(imu_buffer_length)) {
        return false;
    }
    if(!storedOutput.init(imu_buffer_length)) {
        return false;
    }

    return true;
}
    

/********************************************************
*                   INIT FUNCTIONS                      *
********************************************************/

// Use a function call rather than a constructor to initialise variables because it enables the filter to be re-started in flight if necessary.
void NavEKF3_core::InitialiseVariables()
{
    // calculate the nominal filter update rate
    const AP_InertialSensor &ins = _ahrs->get_ins();
    localFilterTimeStep_ms = (uint8_t)(1000*ins.get_loop_delta_t());
    localFilterTimeStep_ms = MAX(localFilterTimeStep_ms,10);

    // initialise time stamps
    imuSampleTime_ms = frontend->imuSampleTime_us / 1000;
    lastHealthyMagTime_ms = imuSampleTime_ms;
    prevTasStep_ms = imuSampleTime_ms;
    prevBetaStep_ms = imuSampleTime_ms;
    lastMagUpdate_us = 0;
    lastBaroReceived_ms = imuSampleTime_ms;
    lastVelPassTime_ms = 0;
    lastPosPassTime_ms = 0;
    lastHgtPassTime_ms = 0;
    lastTasPassTime_ms = 0;
    lastSynthYawTime_ms = imuSampleTime_ms;
    lastTimeGpsReceived_ms = 0;
    secondLastGpsTime_ms = 0;
    lastDecayTime_ms = imuSampleTime_ms;
    timeAtLastAuxEKF_ms = imuSampleTime_ms;
    flowValidMeaTime_ms = imuSampleTime_ms;
    rngValidMeaTime_ms = imuSampleTime_ms;
    flowMeaTime_ms = 0;
    prevFlowFuseTime_ms = 0;
    gndHgtValidTime_ms = 0;
    ekfStartTime_ms = imuSampleTime_ms;
    lastGpsVelFail_ms = 0;
    lastGpsAidBadTime_ms = 0;
    timeTasReceived_ms = 0;
    magYawResetTimer_ms = imuSampleTime_ms;
    lastPreAlignGpsCheckTime_ms = imuSampleTime_ms;
    lastPosReset_ms = 0;
    lastVelReset_ms = 0;
    lastPosResetD_ms = 0;
    lastRngMeasTime_ms = 0;
    terrainHgtStableSet_ms = 0;

    // initialise other variables
    gpsNoiseScaler = 1.0f;
    hgtTimeout = true;
    magTimeout = false;
    allMagSensorsFailed = false;
    tasTimeout = true;
    badMagYaw = false;
    badIMUdata = false;
    finalInflightYawInit = false;
    finalInflightMagInit = false;
    dtIMUavg = 0.0025f;
    dtEkfAvg = EKF_TARGET_DT;
    dt = 0;
    velDotNEDfilt.zero();
    lastKnownPositionNE.zero();
    prevTnb.zero();
    memset(&P[0][0], 0, sizeof(P));
    memset(&nextP[0][0], 0, sizeof(nextP));
    memset(&processNoise[0], 0, sizeof(processNoise));
    flowDataValid = false;
    rangeDataToFuse  = false;
    fuseOptFlowData = false;
    Popt = 0.0f;
    terrainState = 0.0f;
    prevPosN = stateStruct.position.x;
    prevPosE = stateStruct.position.y;
    inhibitGndState = false;
    flowGyroBias.x = 0;
    flowGyroBias.y = 0;
    heldVelNE.zero();
    PV_AidingMode = AID_NONE;
    PV_AidingModePrev = AID_NONE;
    posTimeout = true;
    velTimeout = true;
    memset(&faultStatus, 0, sizeof(faultStatus));
    hgtRate = 0.0f;
    mag_state.q0 = 1;
    mag_state.DCM.identity();
    onGround = true;
    prevOnGround = true;
    inFlight = false;
    prevInFlight = false;
    manoeuvring = false;
    inhibitWindStates = true;
    inhibitMagStates = true;
    gndOffsetValid =  false;
    validOrigin = false;
    takeoffExpectedSet_ms = 0;
    expectGndEffectTakeoff = false;
    touchdownExpectedSet_ms = 0;
    expectGndEffectTouchdown = false;
    gpsSpdAccuracy = 0.0f;
    gpsPosAccuracy = 0.0f;
    gpsHgtAccuracy = 0.0f;
    baroHgtOffset = 0.0f;
    yawResetAngle = 0.0f;
    lastYawReset_ms = 0;
    tiltAlignComplete = false;
    yawAlignComplete = false;
    stateIndexLim = 23;
    baroStoreIndex = 0;
    rangeStoreIndex = 0;
    magStoreIndex = 0;
    gpsStoreIndex = 0;
    tasStoreIndex = 0;
    ofStoreIndex = 0;
    delAngCorrection.zero();
    velErrintegral.zero();
    posErrintegral.zero();
    gpsGoodToAlign = false;
    gpsNotAvailable = true;
    motorsArmed = false;
    prevMotorsArmed = false;
    innovationIncrement = 0;
    lastInnovation = 0;
    memset(&gpsCheckStatus, 0, sizeof(gpsCheckStatus));
    gpsSpdAccPass = false;
    ekfInnovationsPass = false;
    sAccFilterState1 = 0.0f;
    sAccFilterState2 = 0.0f;
    lastGpsCheckTime_ms = 0;
    lastInnovPassTime_ms = 0;
    lastInnovFailTime_ms = 0;
    gpsAccuracyGood = false;
    memset(&gpsloc_prev, 0, sizeof(gpsloc_prev));
    gpsDriftNE = 0.0f;
    gpsVertVelFilt = 0.0f;
    gpsHorizVelFilt = 0.0f;
    memset(&statesArray, 0, sizeof(statesArray));
    posDownDerivative = 0.0f;
    posDown = 0.0f;
    posVelFusionDelayed = false;
    optFlowFusionDelayed = false;
    airSpdFusionDelayed = false;
    sideSlipFusionDelayed = false;
    posResetNE.zero();
    velResetNE.zero();
    posResetD = 0.0f;
    hgtInnovFiltState = 0.0f;
    if (_ahrs->get_compass()) {
        magSelectIndex = _ahrs->get_compass()->get_primary();
    }
    imuDataDownSampledNew.delAng.zero();
    imuDataDownSampledNew.delVel.zero();
    imuDataDownSampledNew.delAngDT = 0.0f;
    imuDataDownSampledNew.delVelDT = 0.0f;
    runUpdates = false;
    framesSincePredict = 0;
    lastMagOffsetsValid = false;
    magStateResetRequest = false;
    magStateInitComplete = false;
    magYawResetRequest = false;
    gpsYawResetRequest = false;
    posDownAtLastMagReset = stateStruct.position.z;
    yawInnovAtLastMagReset = 0.0f;
    quatAtLastMagReset = stateStruct.quat;
    magFieldLearned = false;
    delAngBiasLearned = false;
    memset(&filterStatus, 0, sizeof(filterStatus));
    gpsInhibit = false;
    activeHgtSource = 0;
    memset(&rngMeasIndex, 0, sizeof(rngMeasIndex));
    memset(&storedRngMeasTime_ms, 0, sizeof(storedRngMeasTime_ms));
    memset(&storedRngMeas, 0, sizeof(storedRngMeas));
    terrainHgtStable = true;
    ekfOriginHgtVar = 0.0f;
    velOffsetNED.zero();
    posOffsetNED.zero();
    posResetSource = DEFAULT;
    velResetSource = DEFAULT;

    // range beacon fusion variables
    memset(&rngBcnDataNew, 0, sizeof(rngBcnDataNew));
    memset(&rngBcnDataDelayed, 0, sizeof(rngBcnDataDelayed));
    rngBcnStoreIndex = 0;
    lastRngBcnPassTime_ms = 0;
    rngBcnTestRatio = 0.0f;
    rngBcnHealth = false;
    rngBcnTimeout = true;
    varInnovRngBcn = 0.0f;
    innovRngBcn = 0.0f;
    memset(&lastTimeRngBcn_ms, 0, sizeof(lastTimeRngBcn_ms));
    rngBcnDataToFuse = false;
    beaconVehiclePosNED.zero();
    beaconVehiclePosErr = 1.0f;
    rngBcnLast3DmeasTime_ms = 0;
    rngBcnGoodToAlign = false;
    lastRngBcnChecked = 0;
    receiverPos.zero();
    memset(&receiverPosCov, 0, sizeof(receiverPosCov));
    rngBcnAlignmentStarted =  false;
    rngBcnAlignmentCompleted = false;
    lastBeaconIndex = 0;
    rngBcnPosSum.zero();
    numBcnMeas = 0;
    rngSum = 0.0f;
    N_beacons = 0;
    maxBcnPosD = 0.0f;
    minBcnPosD = 0.0f;
    bcnPosDownOffsetMax = 0.0f;
    bcnPosOffsetMaxVar = 0.0f;
    OffsetMaxInnovFilt = 0.0f;
    bcnPosDownOffsetMin = 0.0f;
    bcnPosOffsetMinVar = 0.0f;
    OffsetMinInnovFilt = 0.0f;
    rngBcnFuseDataReportIndex = 0;
    memset(&rngBcnFusionReport, 0, sizeof(rngBcnFusionReport));
    bcnPosOffsetNED.zero();
    bcnOriginEstInit = false;

    // zero data buffers
    storedIMU.reset();
    storedGPS.reset();
    storedMag.reset();
    storedBaro.reset();
    storedTAS.reset();
    storedRange.reset();
    storedOutput.reset();
    storedRangeBeacon.reset();
}

// Initialise the states from accelerometer and magnetometer data (if present)
// This method can only be used when the vehicle is static
bool NavEKF3_core::InitialiseFilterBootstrap(void)
{
    // If we are a plane and don't have GPS lock then don't initialise
    if (assume_zero_sideslip() && _ahrs->get_gps().status() < AP_GPS::GPS_OK_FIX_3D) {
        statesInitialised = false;
        return false;
    }

    // set re-used variables to zero
    InitialiseVariables();

    // Initialise IMU data
    dtIMUavg = _ahrs->get_ins().get_loop_delta_t();
    readIMUData();
    storedIMU.reset_history(imuDataNew);
    imuDataDelayed = imuDataNew;

    // acceleration vector in XYZ body axes measured by the IMU (m/s^2)
    Vector3f initAccVec;

    // TODO we should average accel readings over several cycles
    initAccVec = _ahrs->get_ins().get_accel(imu_index);

    // read the magnetometer data
    readMagData();

    // normalise the acceleration vector
    float pitch=0, roll=0;
    if (initAccVec.length() > 0.001f) {
        initAccVec.normalize();

        // calculate initial pitch angle
        pitch = asinf(initAccVec.x);

        // calculate initial roll angle
        roll = atan2f(-initAccVec.y , -initAccVec.z);
    }

    // calculate initial roll and pitch orientation
    stateStruct.quat.from_euler(roll, pitch, 0.0f);

    // initialise dynamic states
    stateStruct.velocity.zero();
    stateStruct.position.zero();

    // initialise static process model states
    stateStruct.gyro_bias.zero();
    stateStruct.accel_bias.zero();
    stateStruct.wind_vel.zero();
    stateStruct.earth_magfield.zero();
    stateStruct.body_magfield.zero();

    // read the GPS and set the position and velocity states
    readGpsData();
    ResetVelocity();
    ResetPosition();

    // read the barometer and set the height state
    readBaroData();
    ResetHeight();

    // define Earth rotation vector in the NED navigation frame
    calcEarthRateNED(earthRateNED, _ahrs->get_home().lat);

    // initialise the covariance matrix
    CovarianceInit();

    // reset output states
    StoreOutputReset();

    // set to true now that states have be initialised
    statesInitialised = true;

    return true;
}

// initialise the covariance matrix
void NavEKF3_core::CovarianceInit()
{
    // zero the matrix
    for (uint8_t i=1; i<=stateIndexLim; i++)
    {
        for (uint8_t j=0; j<=stateIndexLim; j++)
        {
            P[i][j] = 0.0f;
        }
    }

    // define the initial angle uncertainty as variances for a rotation vector
    Vector3f rot_vec_var;
    rot_vec_var.x = rot_vec_var.y = rot_vec_var.z = sq(0.1f);

    // update the quaternion state covariances
    initialiseQuatCovariances(rot_vec_var);

    // velocities
    P[4][4]   = sq(frontend->_gpsHorizVelNoise);
    P[5][5]   = P[4][4];
    P[6][6]   = sq(frontend->_gpsVertVelNoise);
    // positions
    P[7][7]   = sq(frontend->_gpsHorizPosNoise);
    P[8][8]   = P[7][7];
    P[9][9]   = sq(frontend->_baroAltNoise);
    // gyro delta angle biases
    P[10][10] = sq(radians(InitialGyroBiasUncertainty() * dtEkfAvg));
    P[11][11] = P[10][10];
    P[12][12] = P[10][10];
    // delta velocity biases
    P[13][13] = sq(ACCEL_BIAS_LIM_SCALER * frontend->_accBiasLim * dtEkfAvg);
    P[14][14] = P[13][13];
    P[15][15] = P[13][13];
    // earth magnetic field
    P[16][16] = 0.0f;
    P[17][17] = P[16][16];
    P[18][18] = P[16][16];
    // body magnetic field
    P[19][19] = 0.0f;
    P[20][20] = P[19][19];
    P[21][21] = P[19][19];
    // wind velocities
    P[22][22] = 0.0f;
    P[23][23]  = P[22][22];


    // optical flow ground height covariance
    Popt = 0.25f;

}

/********************************************************
*                 UPDATE FUNCTIONS                      *
********************************************************/
// Update Filter States - this should be called whenever new IMU data is available
void NavEKF3_core::UpdateFilter(bool predict)
{
    // Set the flag to indicate to the filter that the front-end has given permission for a new state prediction cycle to be started
    startPredictEnabled = predict;

    // don't run filter updates if states have not been initialised
    if (!statesInitialised) {
        return;
    }

    // start the timer used for load measurement
#if EK2_DISABLE_INTERRUPTS
    irqstate_t istate = irqsave();
#endif
    hal.util->perf_begin(_perf_UpdateFilter);

    // TODO - in-flight restart method

    //get starting time for update step
    imuSampleTime_ms = frontend->imuSampleTime_us / 1000;

    // Check arm status and perform required checks and mode changes
    controlFilterModes();

    // read IMU data as delta angles and velocities
    readIMUData();

    // Run the EKF equations to estimate at the fusion time horizon if new IMU data is available in the buffer
    if (runUpdates) {
        // Predict states using IMU data from the delayed time horizon
        UpdateStrapdownEquationsNED();

        // Predict the covariance growth
        CovariancePrediction();

        // Update states using  magnetometer data
        SelectMagFusion();

        // Update states using GPS and altimeter data
        SelectVelPosFusion();

        // Update states using range beacon data
        SelectRngBcnFusion();

        // Update states using optical flow data
        SelectFlowFusion();

        // Update states using airspeed data
        SelectTasFusion();

        // Update states using sideslip constraint assumption for fly-forward vehicles
        SelectBetaFusion();

        // Update the filter status
        updateFilterStatus();
    }

    // Wind output forward from the fusion to output time horizon
    calcOutputStates();

    // stop the timer used for load measurement
    hal.util->perf_end(_perf_UpdateFilter);
#if EK2_DISABLE_INTERRUPTS
    irqrestore(istate);
#endif
}

void NavEKF3_core::correctDeltaAngle(Vector3f &delAng, float delAngDT)
{
    delAng -= stateStruct.gyro_bias * (delAngDT / dtEkfAvg);
}

void NavEKF3_core::correctDeltaVelocity(Vector3f &delVel, float delVelDT)
{
    delVel -= stateStruct.accel_bias * (delVelDT / dtEkfAvg);
}

/*
 * Update the quaternion, velocity and position states using delayed IMU measurements
 * because the EKF is running on a delayed time horizon. Note that the quaternion is
 * not used by the EKF equations, which instead estimate the error in the attitude of
 * the vehicle when each observtion is fused. This attitude error is then used to correct
 * the quaternion.
*/
void NavEKF3_core::UpdateStrapdownEquationsNED()
{
    // update the quaternion states by rotating from the previous attitude through
    // the delta angle rotation quaternion and normalise
    // apply correction for earth's rotation rate
    // % * - and + operators have been overloaded
    stateStruct.quat.rotate(delAngCorrected - prevTnb * earthRateNED*imuDataDelayed.delAngDT);
    stateStruct.quat.normalize();

    // transform body delta velocities to delta velocities in the nav frame
    // use the nav frame from previous time step as the delta velocities
    // have been rotated into that frame
    // * and + operators have been overloaded
    Vector3f delVelNav;  // delta velocity vector in earth axes
    delVelNav  = prevTnb.mul_transpose(delVelCorrected);
    delVelNav.z += GRAVITY_MSS*imuDataDelayed.delVelDT;

    // calculate the body to nav cosine matrix
    stateStruct.quat.inverse().rotation_matrix(prevTnb);

    // calculate the rate of change of velocity (used for launch detect and other functions)
    velDotNED = delVelNav / imuDataDelayed.delVelDT;

    // apply a first order lowpass filter
    velDotNEDfilt = velDotNED * 0.05f + velDotNEDfilt * 0.95f;

    // calculate a magnitude of the filtered nav acceleration (required for GPS
    // variance estimation)
    accNavMag = velDotNEDfilt.length();
    accNavMagHoriz = norm(velDotNEDfilt.x , velDotNEDfilt.y);

    // if we are not aiding, then limit the horizontal magnitude of acceleration
    // to prevent large manoeuvre transients disturbing the attitude
    if ((PV_AidingMode == AID_NONE) && (accNavMagHoriz > 5.0f)) {
        float gain = 5.0f/accNavMagHoriz;
        delVelNav.x *= gain;
        delVelNav.y *= gain;
    }

    // save velocity for use in trapezoidal integration for position calcuation
    Vector3f lastVelocity = stateStruct.velocity;

    // sum delta velocities to get velocity
    stateStruct.velocity += delVelNav;

    // apply a trapezoidal integration to velocities to calculate position
    stateStruct.position += (stateStruct.velocity + lastVelocity) * (imuDataDelayed.delVelDT*0.5f);

    // accumulate the bias delta angle and time since last reset by an OF measurement arrival
    delAngBodyOF += delAngCorrected;
    delTimeOF += imuDataDelayed.delAngDT;

    // limit states to protect against divergence
    ConstrainStates();

    // If main filter velocity states are valid, update the range beacon receiver position states
    if (filterStatus.flags.horiz_vel) {
        receiverPos += (stateStruct.velocity + lastVelocity) * (imuDataDelayed.delVelDT*0.5f);
    }
}

/*
 * Propagate PVA solution forward from the fusion time horizon to the current time horizon
 * using simple observer which performs two functions:
 * 1) Corrects for the delayed time horizon used by the EKF.
 * 2) Applies a LPF to state corrections to prevent 'stepping' in states due to measurement
 * fusion introducing unwanted noise into the control loops.
 * The inspiration for using a complementary filter to correct for time delays in the EKF
 * is based on the work by A Khosravian.
 *
 * “Recursive Attitude Estimation in the Presence of Multi-rate and Multi-delay Vector Measurements”
 * A Khosravian, J Trumpf, R Mahony, T Hamel, Australian National University
*/
void NavEKF3_core::calcOutputStates()
{
    // apply corrections to the IMU data
    Vector3f delAngNewCorrected = imuDataNew.delAng;
    Vector3f delVelNewCorrected = imuDataNew.delVel;
    correctDeltaAngle(delAngNewCorrected, imuDataNew.delAngDT);
    correctDeltaVelocity(delVelNewCorrected, imuDataNew.delVelDT);

    // apply corections to track EKF solution
    Vector3f delAng = delAngNewCorrected + delAngCorrection;

    // convert the rotation vector to its equivalent quaternion
    Quaternion deltaQuat;
    deltaQuat.from_axis_angle(delAng);

    // update the quaternion states by rotating from the previous attitude through
    // the delta angle rotation quaternion and normalise
    outputDataNew.quat *= deltaQuat;
    outputDataNew.quat.normalize();

    // calculate the body to nav cosine matrix
    Matrix3f Tbn_temp;
    outputDataNew.quat.rotation_matrix(Tbn_temp);

    // transform body delta velocities to delta velocities in the nav frame
    Vector3f delVelNav  = Tbn_temp*delVelNewCorrected;
    delVelNav.z += GRAVITY_MSS*imuDataNew.delVelDT;

    // save velocity for use in trapezoidal integration for position calcuation
    Vector3f lastVelocity = outputDataNew.velocity;

    // sum delta velocities to get velocity
    outputDataNew.velocity += delVelNav;

    // apply a trapezoidal integration to velocities to calculate position
    outputDataNew.position += (outputDataNew.velocity + lastVelocity) * (imuDataNew.delVelDT*0.5f);

    // If the IMU accelerometer is offset from the body frame origin, then calculate corrections
    // that can be added to the EKF velocity and position outputs so that they represent the velocity
    // and position of the body frame origin.
    // Note the * operator has been overloaded to operate as a dot product
    if (!accelPosOffset.is_zero()) {
        // calculate the average angular rate across the last IMU update
        // note delAngDT is prevented from being zero in readIMUData()
        Vector3f angRate = imuDataNew.delAng * (1.0f/imuDataNew.delAngDT);

        // Calculate the velocity of the body frame origin relative to the IMU in body frame
        // and rotate into earth frame. Note % operator has been overloaded to perform a cross product
        Vector3f velBodyRelIMU = angRate % (- accelPosOffset);
        velOffsetNED = Tbn_temp * velBodyRelIMU;

        // calculate the earth frame position of the body frame origin relative to the IMU
        posOffsetNED = Tbn_temp * (- accelPosOffset);
    } else {
        velOffsetNED.zero();
        posOffsetNED.zero();
    }

    // store INS states in a ring buffer that with the same length and time coordinates as the IMU data buffer
    if (runUpdates) {
        // store the states at the output time horizon
        storedOutput[storedIMU.get_youngest_index()] = outputDataNew;

        // recall the states from the fusion time horizon
        outputDataDelayed = storedOutput[storedIMU.get_oldest_index()];

        // compare quaternion data with EKF quaternion at the fusion time horizon and calculate correction

        // divide the demanded quaternion by the estimated to get the error
        Quaternion quatErr = stateStruct.quat / outputDataDelayed.quat;

        // Convert to a delta rotation using a small angle approximation
        quatErr.normalize();
        Vector3f deltaAngErr;
        float scaler;
        if (quatErr[0] >= 0.0f) {
            scaler = 2.0f;
        } else {
            scaler = -2.0f;
        }
        deltaAngErr.x = scaler * quatErr[1];
        deltaAngErr.y = scaler * quatErr[2];
        deltaAngErr.z = scaler * quatErr[3];

        // calculate a gain that provides tight tracking of the estimator states and
        // adjust for changes in time delay to maintain consistent damping ratio of ~0.7
        float timeDelay = 1e-3f * (float)(imuDataNew.time_ms - imuDataDelayed.time_ms);
        timeDelay = fmaxf(timeDelay, dtIMUavg);
        float errorGain = 0.5f / timeDelay;

        // calculate a corrrection to the delta angle
        // that will cause the INS to track the EKF quaternions
        delAngCorrection = deltaAngErr * errorGain * dtIMUavg;

        // calculate velocity and position tracking errors
        Vector3f velErr = (stateStruct.velocity - outputDataDelayed.velocity);
        Vector3f posErr = (stateStruct.position - outputDataDelayed.position);

        // collect magnitude tracking error for diagnostics
        outputTrackError.x = deltaAngErr.length();
        outputTrackError.y = velErr.length();
        outputTrackError.z = posErr.length();

        // convert user specified time constant from centi-seconds to seconds
        float tauPosVel = constrain_float(0.01f*(float)frontend->_tauVelPosOutput, 0.1f, 0.5f);

        // calculate a gain to track the EKF position states with the specified time constant
        float velPosGain = dtEkfAvg / constrain_float(tauPosVel, dtEkfAvg, 10.0f);

        // use a PI feedback to calculate a correction that will be applied to the output state history
        posErrintegral += posErr;
        velErrintegral += velErr;
        Vector3f velCorrection = velErr * velPosGain + velErrintegral * sq(velPosGain) * 0.1f;
        Vector3f posCorrection = posErr * velPosGain + posErrintegral * sq(velPosGain) * 0.1f;

        // loop through the output filter state history and apply the corrections to the velocity and position states
        // this method is too expensive to use for the attitude states due to the quaternion operations required
        // but does not introduce a time delay in the 'correction loop' and allows smaller tracking time constants
        // to be used
        output_elements outputStates;
        for (unsigned index=0; index < imu_buffer_length; index++) {
            outputStates = storedOutput[index];

            // a constant  velocity correction is applied
            outputStates.velocity += velCorrection;

            // a constant position correction is applied
            outputStates.position += posCorrection;

            // push the updated data to the buffer
            storedOutput[index] = outputStates;
        }

        // update output state to corrected values
        outputDataNew = storedOutput[storedIMU.get_youngest_index()];

    }
}

/*
 * Calculate the predicted state covariance matrix using algebraic equations generated with Matlab symbolic toolbox.
 * The script file used to generate these and other equations in this filter can be found here:
 * https://github.com/priseborough/InertialNav/blob/master/derivations/RotationVectorAttitudeParameterisation/GenerateNavFilterEquations.m
*/
void NavEKF3_core::CovariancePrediction()
{
    hal.util->perf_begin(_perf_CovariancePrediction);
    float windVelSigma; // wind velocity 1-sigma process noise - m/s
    float dAngBiasSigma;// delta angle bias 1-sigma process noise - rad/s
    float dVelBiasSigma;// delta velocity bias 1-sigma process noise - m/s
    float magEarthSigma;// earth magnetic field 1-sigma process noise
    float magBodySigma; // body magnetic field 1-sigma process noise
    float daxVar;     // X axis delta angle noise variance rad^2
    float dayVar;     // Y axis delta angle noise variance rad^2
    float dazVar;     // Z axis delta angle noise variance rad^2
    float dvxVar;     // X axis delta velocity variance noise (m/s)^2
    float dvyVar;     // Y axis delta velocity variance noise (m/s)^2
    float dvzVar;     // Z axis delta velocity variance noise (m/s)^2
    float dvx;          // X axis delta velocity (m/s)
    float dvy;          // Y axis delta velocity (m/s)
    float dvz;          // Z axis delta velocity (m/s)
    float dax;          // X axis delta angle (rad)
    float day;          // Y axis delta angle (rad)
    float daz;          // Z axis delta angle (rad)
    float q0;           // attitude quaternion
    float q1;           // attitude quaternion
    float q2;           // attitude quaternion
    float q3;           // attitude quaternion
    float dax_b;        // X axis delta angle measurement bias (rad)
    float day_b;        // Y axis delta angle measurement bias (rad)
    float daz_b;        // Z axis delta angle measurement bias (rad)
    float dvx_b;        // X axis delta velocity measurement bias (rad)
    float dvy_b;        // Y axis delta velocity measurement bias (rad)
    float dvz_b;        // Z axis delta velocity measurement bias (rad)

    // calculate covariance prediction process noise
    // use filtered height rate to increase wind process noise when climbing or descending
    // this allows for wind gradient effects.
    // filter height rate using a 10 second time constant filter
    dt = imuDataDelayed.delAngDT;
    float alpha = 0.1f * dt;
    hgtRate = hgtRate * (1.0f - alpha) - stateStruct.velocity.z * alpha;

    // use filtered height rate to increase wind process noise when climbing or descending
    // this allows for wind gradient effects.
    windVelSigma  = dt * constrain_float(frontend->_windVelProcessNoise, 0.0f, 1.0f) * (1.0f + constrain_float(frontend->_wndVarHgtRateScale, 0.0f, 1.0f) * fabsf(hgtRate));
    dAngBiasSigma = sq(dt) * constrain_float(frontend->_gyroBiasProcessNoise, 0.0f, 1.0f);
    dVelBiasSigma = sq(dt) * constrain_float(frontend->_accelBiasProcessNoise, 0.0f, 1.0f);
    magEarthSigma = dt * constrain_float(frontend->_magEarthProcessNoise, 0.0f, 1.0f);
    magBodySigma  = dt * constrain_float(frontend->_magBodyProcessNoise, 0.0f, 1.0f);
    for (uint8_t i= 0; i<=9;  i++) processNoise[i] = 0.0f;
    for (uint8_t i=10; i<=12; i++) processNoise[i] = dAngBiasSigma;
    for (uint8_t i=13; i<=15; i++) processNoise[i] = dVelBiasSigma;
    if (expectGndEffectTakeoff) {
        processNoise[15] = 0.0f;
    } else {
        processNoise[15] = dVelBiasSigma;
    }
    for (uint8_t i=16; i<=18; i++) processNoise[i] = magEarthSigma;
    for (uint8_t i=19; i<=21; i++) processNoise[i] = magBodySigma;
    for (uint8_t i=22; i<=23; i++) processNoise[i] = windVelSigma;

    for (uint8_t i= 0; i<=stateIndexLim; i++) processNoise[i] = sq(processNoise[i]);

    // set variables used to calculate covariance growth
    dvx = imuDataDelayed.delVel.x;
    dvy = imuDataDelayed.delVel.y;
    dvz = imuDataDelayed.delVel.z;
    dax = imuDataDelayed.delAng.x;
    day = imuDataDelayed.delAng.y;
    daz = imuDataDelayed.delAng.z;
    q0 = stateStruct.quat[0];
    q1 = stateStruct.quat[1];
    q2 = stateStruct.quat[2];
    q3 = stateStruct.quat[3];
    dax_b = stateStruct.gyro_bias.x;
    day_b = stateStruct.gyro_bias.y;
    daz_b = stateStruct.gyro_bias.z;
    dvx_b = stateStruct.accel_bias.x;
    dvy_b = stateStruct.accel_bias.y;
    dvz_b = stateStruct.accel_bias.z;
    float _gyrNoise = constrain_float(frontend->_gyrNoise, 0.0f, 1.0f);
    daxVar = dayVar = dazVar = sq(dt*_gyrNoise);
    float _accNoise = constrain_float(frontend->_accNoise, 0.0f, 10.0f);
    dvxVar = dvyVar = dvzVar = sq(dt*_accNoise);

    // calculate the predicted covariance due to inertial sensor error propagation
    // we calculate the upper diagonal and copy to take advantage of symmetry
    SF[0] = dvz - dvz_b;
    SF[1] = dvy - dvy_b;
    SF[2] = dvx - dvx_b;
    SF[3] = 2.0f*q1*SF[2] + 2.0f*q2*SF[1] + 2.0f*q3*SF[0];
    SF[4] = 2.0f*q0*SF[1] - 2.0f*q1*SF[0] + 2.0f*q3*SF[2];
    SF[5] = 2.0f*q0*SF[2] + 2.0f*q2*SF[0] - 2.0f*q3*SF[1];
    SF[6] = day*0.5f - day_b*0.5f;
    SF[7] = daz*0.5f - daz_b*0.5f;
    SF[8] = dax*0.5f - dax_b*0.5f;
    SF[9] = dax_b*0.5f - dax*0.5f;
    SF[10] = daz_b*0.5f - daz*0.5f;
    SF[11] = day_b*0.5f - day*0.5f;
    SF[12] = 2.0f*q1*SF[1];
    SF[13] = 2.0f*q0*SF[0];
    SF[14] = q1*0.5f;
    SF[15] = q2*0.5f;
    SF[16] = q3*0.5f;
    SF[17] = sq(q3);
    SF[18] = sq(q2);
    SF[19] = sq(q1);
    SF[20] = sq(q0);

    SG[0] = q0*0.5f;
    SG[1] = sq(q3);
    SG[2] = sq(q2);
    SG[3] = sq(q1);
    SG[4] = sq(q0);
    SG[5] = 2.0f*q2*q3;
    SG[6] = 2.0f*q1*q3;
    SG[7] = 2.0f*q1*q2;

    SQ[0] = dvzVar*(SG[5] - 2.0f*q0*q1)*(SG[1] - SG[2] - SG[3] + SG[4]) - dvyVar*(SG[5] + 2.0f*q0*q1)*(SG[1] - SG[2] + SG[3] - SG[4]) + dvxVar*(SG[6] - 2.0f*q0*q2)*(SG[7] + 2.0f*q0*q3);
    SQ[1] = dvzVar*(SG[6] + 2.0f*q0*q2)*(SG[1] - SG[2] - SG[3] + SG[4]) - dvxVar*(SG[6] - 2.0f*q0*q2)*(SG[1] + SG[2] - SG[3] - SG[4]) + dvyVar*(SG[5] + 2.0f*q0*q1)*(SG[7] - 2.0f*q0*q3);
    SQ[2] = dvzVar*(SG[5] - 2.0f*q0*q1)*(SG[6] + 2.0f*q0*q2) - dvyVar*(SG[7] - 2.0f*q0*q3)*(SG[1] - SG[2] + SG[3] - SG[4]) - dvxVar*(SG[7] + 2.0f*q0*q3)*(SG[1] + SG[2] - SG[3] - SG[4]);
    SQ[3] = (dayVar*q1*SG[0])*0.5f - (dazVar*q1*SG[0])*0.5f - (daxVar*q2*q3)*0.25f;
    SQ[4] = (dazVar*q2*SG[0])*0.5f - (daxVar*q2*SG[0])*0.5f - (dayVar*q1*q3)*0.25f;
    SQ[5] = (daxVar*q3*SG[0])*0.5f - (dayVar*q3*SG[0])*0.5f - (dazVar*q1*q2)*0.25f;
    SQ[6] = (daxVar*q1*q2)*0.25f - (dazVar*q3*SG[0])*0.5f - (dayVar*q1*q2)*0.25f;
    SQ[7] = (dazVar*q1*q3)*0.25f - (daxVar*q1*q3)*0.25f - (dayVar*q2*SG[0])*0.5f;
    SQ[8] = (dayVar*q2*q3)*0.25f - (daxVar*q1*SG[0])*0.5f - (dazVar*q2*q3)*0.25f;
    SQ[9] = sq(SG[0]);
    SQ[10] = sq(q1);

    SPP[0] = SF[12] + SF[13] - 2.0f*q2*SF[2];
    SPP[1] = SF[17] - SF[18] - SF[19] + SF[20];
    SPP[2] = SF[17] - SF[18] + SF[19] - SF[20];
    SPP[3] = SF[17] + SF[18] - SF[19] - SF[20];
    SPP[4] = 2.0f*q0*q2 - 2.0f*q1*q3;
    SPP[5] = 2.0f*q0*q1 - 2.0f*q2*q3;
    SPP[6] = 2.0f*q0*q3 - 2.0f*q1*q2;
    SPP[7] = 2.0f*q0*q1 + 2.0f*q2*q3;
    SPP[8] = 2.0f*q0*q3 + 2.0f*q1*q2;
    SPP[9] = 2.0f*q0*q2 + 2.0f*q1*q3;
    SPP[10] = SF[16];

    if (inhibitDelAngBiasStates) {
        zeroRows(P,10,12);
        zeroCols(P,10,12);
    }

    if (inhibitDelVelBiasStates) {
        zeroRows(P,13,15);
        zeroCols(P,13,15);
    }

    if (inhibitMagStates) {
        zeroRows(P,16,21);
        zeroCols(P,16,21);
    }

    if (inhibitWindStates) {
        zeroRows(P,22,23);
        zeroCols(P,22,23);
    }

    nextP[0][0] = P[0][0] + P[1][0]*SF[9] + P[2][0]*SF[11] + P[3][0]*SF[10] + P[10][0]*SF[14] + P[11][0]*SF[15] + P[12][0]*SPP[10] + (daxVar*SQ[10])*0.25f + SF[9]*(P[0][1] + P[1][1]*SF[9] + P[2][1]*SF[11] + P[3][1]*SF[10] + P[10][1]*SF[14] + P[11][1]*SF[15] + P[12][1]*SPP[10]) + SF[11]*(P[0][2] + P[1][2]*SF[9] + P[2][2]*SF[11] + P[3][2]*SF[10] + P[10][2]*SF[14] + P[11][2]*SF[15] + P[12][2]*SPP[10]) + SF[10]*(P[0][3] + P[1][3]*SF[9] + P[2][3]*SF[11] + P[3][3]*SF[10] + P[10][3]*SF[14] + P[11][3]*SF[15] + P[12][3]*SPP[10]) + SF[14]*(P[0][10] + P[1][10]*SF[9] + P[2][10]*SF[11] + P[3][10]*SF[10] + P[10][10]*SF[14] + P[11][10]*SF[15] + P[12][10]*SPP[10]) + SF[15]*(P[0][11] + P[1][11]*SF[9] + P[2][11]*SF[11] + P[3][11]*SF[10] + P[10][11]*SF[14] + P[11][11]*SF[15] + P[12][11]*SPP[10]) + SPP[10]*(P[0][12] + P[1][12]*SF[9] + P[2][12]*SF[11] + P[3][12]*SF[10] + P[10][12]*SF[14] + P[11][12]*SF[15] + P[12][12]*SPP[10]) + (dayVar*sq(q2))*0.25f + (dazVar*sq(q3))*0.25f;
    nextP[0][1] = P[0][1] + SQ[8] + P[1][1]*SF[9] + P[2][1]*SF[11] + P[3][1]*SF[10] + P[10][1]*SF[14] + P[11][1]*SF[15] + P[12][1]*SPP[10] + SF[8]*(P[0][0] + P[1][0]*SF[9] + P[2][0]*SF[11] + P[3][0]*SF[10] + P[10][0]*SF[14] + P[11][0]*SF[15] + P[12][0]*SPP[10]) + SF[7]*(P[0][2] + P[1][2]*SF[9] + P[2][2]*SF[11] + P[3][2]*SF[10] + P[10][2]*SF[14] + P[11][2]*SF[15] + P[12][2]*SPP[10]) + SF[11]*(P[0][3] + P[1][3]*SF[9] + P[2][3]*SF[11] + P[3][3]*SF[10] + P[10][3]*SF[14] + P[11][3]*SF[15] + P[12][3]*SPP[10]) - SF[15]*(P[0][12] + P[1][12]*SF[9] + P[2][12]*SF[11] + P[3][12]*SF[10] + P[10][12]*SF[14] + P[11][12]*SF[15] + P[12][12]*SPP[10]) + SPP[10]*(P[0][11] + P[1][11]*SF[9] + P[2][11]*SF[11] + P[3][11]*SF[10] + P[10][11]*SF[14] + P[11][11]*SF[15] + P[12][11]*SPP[10]) - (q0*(P[0][10] + P[1][10]*SF[9] + P[2][10]*SF[11] + P[3][10]*SF[10] + P[10][10]*SF[14] + P[11][10]*SF[15] + P[12][10]*SPP[10]))*0.5f;
    nextP[1][1] = P[1][1] + P[0][1]*SF[8] + P[2][1]*SF[7] + P[3][1]*SF[11] - P[12][1]*SF[15] + P[11][1]*SPP[10] + daxVar*SQ[9] - (P[10][1]*q0)*0.5f + SF[8]*(P[1][0] + P[0][0]*SF[8] + P[2][0]*SF[7] + P[3][0]*SF[11] - P[12][0]*SF[15] + P[11][0]*SPP[10] - (P[10][0]*q0)/2) + SF[7]*(P[1][2] + P[0][2]*SF[8] + P[2][2]*SF[7] + P[3][2]*SF[11] - P[12][2]*SF[15] + P[11][2]*SPP[10] - (P[10][2]*q0)/2) + SF[11]*(P[1][3] + P[0][3]*SF[8] + P[2][3]*SF[7] + P[3][3]*SF[11] - P[12][3]*SF[15] + P[11][3]*SPP[10] - (P[10][3]*q0)/2) - SF[15]*(P[1][12] + P[0][12]*SF[8] + P[2][12]*SF[7] + P[3][12]*SF[11] - P[12][12]*SF[15] + P[11][12]*SPP[10] - (P[10][12]*q0)/2) + SPP[10]*(P[1][11] + P[0][11]*SF[8] + P[2][11]*SF[7] + P[3][11]*SF[11] - P[12][11]*SF[15] + P[11][11]*SPP[10] - (P[10][11]*q0)/2) + (dayVar*sq(q3))*0.25f + (dazVar*sq(q2))*0.25f - (q0*(P[1][10] + P[0][10]*SF[8] + P[2][10]*SF[7] + P[3][10]*SF[11] - P[12][10]*SF[15] + P[11][10]*SPP[10] - (P[10][10]*q0)/2))*0.5f;
    nextP[0][2] = P[0][2] + SQ[7] + P[1][2]*SF[9] + P[2][2]*SF[11] + P[3][2]*SF[10] + P[10][2]*SF[14] + P[11][2]*SF[15] + P[12][2]*SPP[10] + SF[6]*(P[0][0] + P[1][0]*SF[9] + P[2][0]*SF[11] + P[3][0]*SF[10] + P[10][0]*SF[14] + P[11][0]*SF[15] + P[12][0]*SPP[10]) + SF[10]*(P[0][1] + P[1][1]*SF[9] + P[2][1]*SF[11] + P[3][1]*SF[10] + P[10][1]*SF[14] + P[11][1]*SF[15] + P[12][1]*SPP[10]) + SF[8]*(P[0][3] + P[1][3]*SF[9] + P[2][3]*SF[11] + P[3][3]*SF[10] + P[10][3]*SF[14] + P[11][3]*SF[15] + P[12][3]*SPP[10]) + SF[14]*(P[0][12] + P[1][12]*SF[9] + P[2][12]*SF[11] + P[3][12]*SF[10] + P[10][12]*SF[14] + P[11][12]*SF[15] + P[12][12]*SPP[10]) - SPP[10]*(P[0][10] + P[1][10]*SF[9] + P[2][10]*SF[11] + P[3][10]*SF[10] + P[10][10]*SF[14] + P[11][10]*SF[15] + P[12][10]*SPP[10]) - (q0*(P[0][11] + P[1][11]*SF[9] + P[2][11]*SF[11] + P[3][11]*SF[10] + P[10][11]*SF[14] + P[11][11]*SF[15] + P[12][11]*SPP[10]))*0.5f;
    nextP[1][2] = P[1][2] + SQ[5] + P[0][2]*SF[8] + P[2][2]*SF[7] + P[3][2]*SF[11] - P[12][2]*SF[15] + P[11][2]*SPP[10] - (P[10][2]*q0)*0.5f + SF[6]*(P[1][0] + P[0][0]*SF[8] + P[2][0]*SF[7] + P[3][0]*SF[11] - P[12][0]*SF[15] + P[11][0]*SPP[10] - (P[10][0]*q0)/2) + SF[10]*(P[1][1] + P[0][1]*SF[8] + P[2][1]*SF[7] + P[3][1]*SF[11] - P[12][1]*SF[15] + P[11][1]*SPP[10] - (P[10][1]*q0)/2) + SF[8]*(P[1][3] + P[0][3]*SF[8] + P[2][3]*SF[7] + P[3][3]*SF[11] - P[12][3]*SF[15] + P[11][3]*SPP[10] - (P[10][3]*q0)/2) + SF[14]*(P[1][12] + P[0][12]*SF[8] + P[2][12]*SF[7] + P[3][12]*SF[11] - P[12][12]*SF[15] + P[11][12]*SPP[10] - (P[10][12]*q0)/2) - SPP[10]*(P[1][10] + P[0][10]*SF[8] + P[2][10]*SF[7] + P[3][10]*SF[11] - P[12][10]*SF[15] + P[11][10]*SPP[10] - (P[10][10]*q0)/2) - (q0*(P[1][11] + P[0][11]*SF[8] + P[2][11]*SF[7] + P[3][11]*SF[11] - P[12][11]*SF[15] + P[11][11]*SPP[10] - (P[10][11]*q0)/2))*0.5f;
    nextP[2][2] = P[2][2] + P[0][2]*SF[6] + P[1][2]*SF[10] + P[3][2]*SF[8] + P[12][2]*SF[14] - P[10][2]*SPP[10] + dayVar*SQ[9] + (dazVar*SQ[10])*0.25f - (P[11][2]*q0)*0.5f + SF[6]*(P[2][0] + P[0][0]*SF[6] + P[1][0]*SF[10] + P[3][0]*SF[8] + P[12][0]*SF[14] - P[10][0]*SPP[10] - (P[11][0]*q0)/2) + SF[10]*(P[2][1] + P[0][1]*SF[6] + P[1][1]*SF[10] + P[3][1]*SF[8] + P[12][1]*SF[14] - P[10][1]*SPP[10] - (P[11][1]*q0)/2) + SF[8]*(P[2][3] + P[0][3]*SF[6] + P[1][3]*SF[10] + P[3][3]*SF[8] + P[12][3]*SF[14] - P[10][3]*SPP[10] - (P[11][3]*q0)/2) + SF[14]*(P[2][12] + P[0][12]*SF[6] + P[1][12]*SF[10] + P[3][12]*SF[8] + P[12][12]*SF[14] - P[10][12]*SPP[10] - (P[11][12]*q0)/2) - SPP[10]*(P[2][10] + P[0][10]*SF[6] + P[1][10]*SF[10] + P[3][10]*SF[8] + P[12][10]*SF[14] - P[10][10]*SPP[10] - (P[11][10]*q0)/2) + (daxVar*sq(q3))*0.25f - (q0*(P[2][11] + P[0][11]*SF[6] + P[1][11]*SF[10] + P[3][11]*SF[8] + P[12][11]*SF[14] - P[10][11]*SPP[10] - (P[11][11]*q0)/2))*0.5f;
    nextP[0][3] = P[0][3] + SQ[6] + P[1][3]*SF[9] + P[2][3]*SF[11] + P[3][3]*SF[10] + P[10][3]*SF[14] + P[11][3]*SF[15] + P[12][3]*SPP[10] + SF[7]*(P[0][0] + P[1][0]*SF[9] + P[2][0]*SF[11] + P[3][0]*SF[10] + P[10][0]*SF[14] + P[11][0]*SF[15] + P[12][0]*SPP[10]) + SF[6]*(P[0][1] + P[1][1]*SF[9] + P[2][1]*SF[11] + P[3][1]*SF[10] + P[10][1]*SF[14] + P[11][1]*SF[15] + P[12][1]*SPP[10]) + SF[9]*(P[0][2] + P[1][2]*SF[9] + P[2][2]*SF[11] + P[3][2]*SF[10] + P[10][2]*SF[14] + P[11][2]*SF[15] + P[12][2]*SPP[10]) + SF[15]*(P[0][10] + P[1][10]*SF[9] + P[2][10]*SF[11] + P[3][10]*SF[10] + P[10][10]*SF[14] + P[11][10]*SF[15] + P[12][10]*SPP[10]) - SF[14]*(P[0][11] + P[1][11]*SF[9] + P[2][11]*SF[11] + P[3][11]*SF[10] + P[10][11]*SF[14] + P[11][11]*SF[15] + P[12][11]*SPP[10]) - (q0*(P[0][12] + P[1][12]*SF[9] + P[2][12]*SF[11] + P[3][12]*SF[10] + P[10][12]*SF[14] + P[11][12]*SF[15] + P[12][12]*SPP[10]))*0.5f;
    nextP[1][3] = P[1][3] + SQ[4] + P[0][3]*SF[8] + P[2][3]*SF[7] + P[3][3]*SF[11] - P[12][3]*SF[15] + P[11][3]*SPP[10] - (P[10][3]*q0)*0.5f + SF[7]*(P[1][0] + P[0][0]*SF[8] + P[2][0]*SF[7] + P[3][0]*SF[11] - P[12][0]*SF[15] + P[11][0]*SPP[10] - (P[10][0]*q0)/2) + SF[6]*(P[1][1] + P[0][1]*SF[8] + P[2][1]*SF[7] + P[3][1]*SF[11] - P[12][1]*SF[15] + P[11][1]*SPP[10] - (P[10][1]*q0)/2) + SF[9]*(P[1][2] + P[0][2]*SF[8] + P[2][2]*SF[7] + P[3][2]*SF[11] - P[12][2]*SF[15] + P[11][2]*SPP[10] - (P[10][2]*q0)/2) + SF[15]*(P[1][10] + P[0][10]*SF[8] + P[2][10]*SF[7] + P[3][10]*SF[11] - P[12][10]*SF[15] + P[11][10]*SPP[10] - (P[10][10]*q0)/2) - SF[14]*(P[1][11] + P[0][11]*SF[8] + P[2][11]*SF[7] + P[3][11]*SF[11] - P[12][11]*SF[15] + P[11][11]*SPP[10] - (P[10][11]*q0)/2) - (q0*(P[1][12] + P[0][12]*SF[8] + P[2][12]*SF[7] + P[3][12]*SF[11] - P[12][12]*SF[15] + P[11][12]*SPP[10] - (P[10][12]*q0)/2))*0.5f;
    nextP[2][3] = P[2][3] + SQ[3] + P[0][3]*SF[6] + P[1][3]*SF[10] + P[3][3]*SF[8] + P[12][3]*SF[14] - P[10][3]*SPP[10] - (P[11][3]*q0)*0.5f + SF[7]*(P[2][0] + P[0][0]*SF[6] + P[1][0]*SF[10] + P[3][0]*SF[8] + P[12][0]*SF[14] - P[10][0]*SPP[10] - (P[11][0]*q0)/2) + SF[6]*(P[2][1] + P[0][1]*SF[6] + P[1][1]*SF[10] + P[3][1]*SF[8] + P[12][1]*SF[14] - P[10][1]*SPP[10] - (P[11][1]*q0)/2) + SF[9]*(P[2][2] + P[0][2]*SF[6] + P[1][2]*SF[10] + P[3][2]*SF[8] + P[12][2]*SF[14] - P[10][2]*SPP[10] - (P[11][2]*q0)/2) + SF[15]*(P[2][10] + P[0][10]*SF[6] + P[1][10]*SF[10] + P[3][10]*SF[8] + P[12][10]*SF[14] - P[10][10]*SPP[10] - (P[11][10]*q0)/2) - SF[14]*(P[2][11] + P[0][11]*SF[6] + P[1][11]*SF[10] + P[3][11]*SF[8] + P[12][11]*SF[14] - P[10][11]*SPP[10] - (P[11][11]*q0)/2) - (q0*(P[2][12] + P[0][12]*SF[6] + P[1][12]*SF[10] + P[3][12]*SF[8] + P[12][12]*SF[14] - P[10][12]*SPP[10] - (P[11][12]*q0)/2))*0.5f;
    nextP[3][3] = P[3][3] + P[0][3]*SF[7] + P[1][3]*SF[6] + P[2][3]*SF[9] + P[10][3]*SF[15] - P[11][3]*SF[14] + (dayVar*SQ[10])*0.25f + dazVar*SQ[9] - (P[12][3]*q0)*0.5f + SF[7]*(P[3][0] + P[0][0]*SF[7] + P[1][0]*SF[6] + P[2][0]*SF[9] + P[10][0]*SF[15] - P[11][0]*SF[14] - (P[12][0]*q0)/2) + SF[6]*(P[3][1] + P[0][1]*SF[7] + P[1][1]*SF[6] + P[2][1]*SF[9] + P[10][1]*SF[15] - P[11][1]*SF[14] - (P[12][1]*q0)/2) + SF[9]*(P[3][2] + P[0][2]*SF[7] + P[1][2]*SF[6] + P[2][2]*SF[9] + P[10][2]*SF[15] - P[11][2]*SF[14] - (P[12][2]*q0)/2) + SF[15]*(P[3][10] + P[0][10]*SF[7] + P[1][10]*SF[6] + P[2][10]*SF[9] + P[10][10]*SF[15] - P[11][10]*SF[14] - (P[12][10]*q0)/2) - SF[14]*(P[3][11] + P[0][11]*SF[7] + P[1][11]*SF[6] + P[2][11]*SF[9] + P[10][11]*SF[15] - P[11][11]*SF[14] - (P[12][11]*q0)/2) + (daxVar*sq(q2))*0.25f - (q0*(P[3][12] + P[0][12]*SF[7] + P[1][12]*SF[6] + P[2][12]*SF[9] + P[10][12]*SF[15] - P[11][12]*SF[14] - (P[12][12]*q0)/2))*0.5f;
    nextP[0][4] = P[0][4] + P[1][4]*SF[9] + P[2][4]*SF[11] + P[3][4]*SF[10] + P[10][4]*SF[14] + P[11][4]*SF[15] + P[12][4]*SPP[10] + SF[5]*(P[0][0] + P[1][0]*SF[9] + P[2][0]*SF[11] + P[3][0]*SF[10] + P[10][0]*SF[14] + P[11][0]*SF[15] + P[12][0]*SPP[10]) + SF[3]*(P[0][1] + P[1][1]*SF[9] + P[2][1]*SF[11] + P[3][1]*SF[10] + P[10][1]*SF[14] + P[11][1]*SF[15] + P[12][1]*SPP[10]) - SF[4]*(P[0][3] + P[1][3]*SF[9] + P[2][3]*SF[11] + P[3][3]*SF[10] + P[10][3]*SF[14] + P[11][3]*SF[15] + P[12][3]*SPP[10]) + SPP[0]*(P[0][2] + P[1][2]*SF[9] + P[2][2]*SF[11] + P[3][2]*SF[10] + P[10][2]*SF[14] + P[11][2]*SF[15] + P[12][2]*SPP[10]) + SPP[3]*(P[0][13] + P[1][13]*SF[9] + P[2][13]*SF[11] + P[3][13]*SF[10] + P[10][13]*SF[14] + P[11][13]*SF[15] + P[12][13]*SPP[10]) + SPP[6]*(P[0][14] + P[1][14]*SF[9] + P[2][14]*SF[11] + P[3][14]*SF[10] + P[10][14]*SF[14] + P[11][14]*SF[15] + P[12][14]*SPP[10]) - SPP[9]*(P[0][15] + P[1][15]*SF[9] + P[2][15]*SF[11] + P[3][15]*SF[10] + P[10][15]*SF[14] + P[11][15]*SF[15] + P[12][15]*SPP[10]);
    nextP[1][4] = P[1][4] + P[0][4]*SF[8] + P[2][4]*SF[7] + P[3][4]*SF[11] - P[12][4]*SF[15] + P[11][4]*SPP[10] - (P[10][4]*q0)*0.5f + SF[5]*(P[1][0] + P[0][0]*SF[8] + P[2][0]*SF[7] + P[3][0]*SF[11] - P[12][0]*SF[15] + P[11][0]*SPP[10] - (P[10][0]*q0)/2) + SF[3]*(P[1][1] + P[0][1]*SF[8] + P[2][1]*SF[7] + P[3][1]*SF[11] - P[12][1]*SF[15] + P[11][1]*SPP[10] - (P[10][1]*q0)/2) - SF[4]*(P[1][3] + P[0][3]*SF[8] + P[2][3]*SF[7] + P[3][3]*SF[11] - P[12][3]*SF[15] + P[11][3]*SPP[10] - (P[10][3]*q0)/2) + SPP[0]*(P[1][2] + P[0][2]*SF[8] + P[2][2]*SF[7] + P[3][2]*SF[11] - P[12][2]*SF[15] + P[11][2]*SPP[10] - (P[10][2]*q0)/2) + SPP[3]*(P[1][13] + P[0][13]*SF[8] + P[2][13]*SF[7] + P[3][13]*SF[11] - P[12][13]*SF[15] + P[11][13]*SPP[10] - (P[10][13]*q0)/2) + SPP[6]*(P[1][14] + P[0][14]*SF[8] + P[2][14]*SF[7] + P[3][14]*SF[11] - P[12][14]*SF[15] + P[11][14]*SPP[10] - (P[10][14]*q0)/2) - SPP[9]*(P[1][15] + P[0][15]*SF[8] + P[2][15]*SF[7] + P[3][15]*SF[11] - P[12][15]*SF[15] + P[11][15]*SPP[10] - (P[10][15]*q0)/2);
    nextP[2][4] = P[2][4] + P[0][4]*SF[6] + P[1][4]*SF[10] + P[3][4]*SF[8] + P[12][4]*SF[14] - P[10][4]*SPP[10] - (P[11][4]*q0)*0.5f + SF[5]*(P[2][0] + P[0][0]*SF[6] + P[1][0]*SF[10] + P[3][0]*SF[8] + P[12][0]*SF[14] - P[10][0]*SPP[10] - (P[11][0]*q0)/2) + SF[3]*(P[2][1] + P[0][1]*SF[6] + P[1][1]*SF[10] + P[3][1]*SF[8] + P[12][1]*SF[14] - P[10][1]*SPP[10] - (P[11][1]*q0)/2) - SF[4]*(P[2][3] + P[0][3]*SF[6] + P[1][3]*SF[10] + P[3][3]*SF[8] + P[12][3]*SF[14] - P[10][3]*SPP[10] - (P[11][3]*q0)/2) + SPP[0]*(P[2][2] + P[0][2]*SF[6] + P[1][2]*SF[10] + P[3][2]*SF[8] + P[12][2]*SF[14] - P[10][2]*SPP[10] - (P[11][2]*q0)/2) + SPP[3]*(P[2][13] + P[0][13]*SF[6] + P[1][13]*SF[10] + P[3][13]*SF[8] + P[12][13]*SF[14] - P[10][13]*SPP[10] - (P[11][13]*q0)/2) + SPP[6]*(P[2][14] + P[0][14]*SF[6] + P[1][14]*SF[10] + P[3][14]*SF[8] + P[12][14]*SF[14] - P[10][14]*SPP[10] - (P[11][14]*q0)/2) - SPP[9]*(P[2][15] + P[0][15]*SF[6] + P[1][15]*SF[10] + P[3][15]*SF[8] + P[12][15]*SF[14] - P[10][15]*SPP[10] - (P[11][15]*q0)/2);
    nextP[3][4] = P[3][4] + P[0][4]*SF[7] + P[1][4]*SF[6] + P[2][4]*SF[9] + P[10][4]*SF[15] - P[11][4]*SF[14] - (P[12][4]*q0)*0.5f + SF[5]*(P[3][0] + P[0][0]*SF[7] + P[1][0]*SF[6] + P[2][0]*SF[9] + P[10][0]*SF[15] - P[11][0]*SF[14] - (P[12][0]*q0)/2) + SF[3]*(P[3][1] + P[0][1]*SF[7] + P[1][1]*SF[6] + P[2][1]*SF[9] + P[10][1]*SF[15] - P[11][1]*SF[14] - (P[12][1]*q0)/2) - SF[4]*(P[3][3] + P[0][3]*SF[7] + P[1][3]*SF[6] + P[2][3]*SF[9] + P[10][3]*SF[15] - P[11][3]*SF[14] - (P[12][3]*q0)/2) + SPP[0]*(P[3][2] + P[0][2]*SF[7] + P[1][2]*SF[6] + P[2][2]*SF[9] + P[10][2]*SF[15] - P[11][2]*SF[14] - (P[12][2]*q0)/2) + SPP[3]*(P[3][13] + P[0][13]*SF[7] + P[1][13]*SF[6] + P[2][13]*SF[9] + P[10][13]*SF[15] - P[11][13]*SF[14] - (P[12][13]*q0)/2) + SPP[6]*(P[3][14] + P[0][14]*SF[7] + P[1][14]*SF[6] + P[2][14]*SF[9] + P[10][14]*SF[15] - P[11][14]*SF[14] - (P[12][14]*q0)/2) - SPP[9]*(P[3][15] + P[0][15]*SF[7] + P[1][15]*SF[6] + P[2][15]*SF[9] + P[10][15]*SF[15] - P[11][15]*SF[14] - (P[12][15]*q0)/2);
    nextP[4][4] = P[4][4] + P[0][4]*SF[5] + P[1][4]*SF[3] - P[3][4]*SF[4] + P[2][4]*SPP[0] + P[13][4]*SPP[3] + P[14][4]*SPP[6] - P[15][4]*SPP[9] + dvyVar*sq(SG[7] - 2.0f*q0*q3) + dvzVar*sq(SG[6] + 2.0f*q0*q2) + SF[5]*(P[4][0] + P[0][0]*SF[5] + P[1][0]*SF[3] - P[3][0]*SF[4] + P[2][0]*SPP[0] + P[13][0]*SPP[3] + P[14][0]*SPP[6] - P[15][0]*SPP[9]) + SF[3]*(P[4][1] + P[0][1]*SF[5] + P[1][1]*SF[3] - P[3][1]*SF[4] + P[2][1]*SPP[0] + P[13][1]*SPP[3] + P[14][1]*SPP[6] - P[15][1]*SPP[9]) - SF[4]*(P[4][3] + P[0][3]*SF[5] + P[1][3]*SF[3] - P[3][3]*SF[4] + P[2][3]*SPP[0] + P[13][3]*SPP[3] + P[14][3]*SPP[6] - P[15][3]*SPP[9]) + SPP[0]*(P[4][2] + P[0][2]*SF[5] + P[1][2]*SF[3] - P[3][2]*SF[4] + P[2][2]*SPP[0] + P[13][2]*SPP[3] + P[14][2]*SPP[6] - P[15][2]*SPP[9]) + SPP[3]*(P[4][13] + P[0][13]*SF[5] + P[1][13]*SF[3] - P[3][13]*SF[4] + P[2][13]*SPP[0] + P[13][13]*SPP[3] + P[14][13]*SPP[6] - P[15][13]*SPP[9]) + SPP[6]*(P[4][14] + P[0][14]*SF[5] + P[1][14]*SF[3] - P[3][14]*SF[4] + P[2][14]*SPP[0] + P[13][14]*SPP[3] + P[14][14]*SPP[6] - P[15][14]*SPP[9]) - SPP[9]*(P[4][15] + P[0][15]*SF[5] + P[1][15]*SF[3] - P[3][15]*SF[4] + P[2][15]*SPP[0] + P[13][15]*SPP[3] + P[14][15]*SPP[6] - P[15][15]*SPP[9]) + dvxVar*sq(SG[1] + SG[2] - SG[3] - SG[4]);
    nextP[0][5] = P[0][5] + P[1][5]*SF[9] + P[2][5]*SF[11] + P[3][5]*SF[10] + P[10][5]*SF[14] + P[11][5]*SF[15] + P[12][5]*SPP[10] + SF[4]*(P[0][0] + P[1][0]*SF[9] + P[2][0]*SF[11] + P[3][0]*SF[10] + P[10][0]*SF[14] + P[11][0]*SF[15] + P[12][0]*SPP[10]) + SF[3]*(P[0][2] + P[1][2]*SF[9] + P[2][2]*SF[11] + P[3][2]*SF[10] + P[10][2]*SF[14] + P[11][2]*SF[15] + P[12][2]*SPP[10]) + SF[5]*(P[0][3] + P[1][3]*SF[9] + P[2][3]*SF[11] + P[3][3]*SF[10] + P[10][3]*SF[14] + P[11][3]*SF[15] + P[12][3]*SPP[10]) - SPP[0]*(P[0][1] + P[1][1]*SF[9] + P[2][1]*SF[11] + P[3][1]*SF[10] + P[10][1]*SF[14] + P[11][1]*SF[15] + P[12][1]*SPP[10]) - SPP[8]*(P[0][13] + P[1][13]*SF[9] + P[2][13]*SF[11] + P[3][13]*SF[10] + P[10][13]*SF[14] + P[11][13]*SF[15] + P[12][13]*SPP[10]) + SPP[2]*(P[0][14] + P[1][14]*SF[9] + P[2][14]*SF[11] + P[3][14]*SF[10] + P[10][14]*SF[14] + P[11][14]*SF[15] + P[12][14]*SPP[10]) + SPP[5]*(P[0][15] + P[1][15]*SF[9] + P[2][15]*SF[11] + P[3][15]*SF[10] + P[10][15]*SF[14] + P[11][15]*SF[15] + P[12][15]*SPP[10]);
    nextP[1][5] = P[1][5] + P[0][5]*SF[8] + P[2][5]*SF[7] + P[3][5]*SF[11] - P[12][5]*SF[15] + P[11][5]*SPP[10] - (P[10][5]*q0)*0.5f + SF[4]*(P[1][0] + P[0][0]*SF[8] + P[2][0]*SF[7] + P[3][0]*SF[11] - P[12][0]*SF[15] + P[11][0]*SPP[10] - (P[10][0]*q0)/2) + SF[3]*(P[1][2] + P[0][2]*SF[8] + P[2][2]*SF[7] + P[3][2]*SF[11] - P[12][2]*SF[15] + P[11][2]*SPP[10] - (P[10][2]*q0)/2) + SF[5]*(P[1][3] + P[0][3]*SF[8] + P[2][3]*SF[7] + P[3][3]*SF[11] - P[12][3]*SF[15] + P[11][3]*SPP[10] - (P[10][3]*q0)/2) - SPP[0]*(P[1][1] + P[0][1]*SF[8] + P[2][1]*SF[7] + P[3][1]*SF[11] - P[12][1]*SF[15] + P[11][1]*SPP[10] - (P[10][1]*q0)/2) - SPP[8]*(P[1][13] + P[0][13]*SF[8] + P[2][13]*SF[7] + P[3][13]*SF[11] - P[12][13]*SF[15] + P[11][13]*SPP[10] - (P[10][13]*q0)/2) + SPP[2]*(P[1][14] + P[0][14]*SF[8] + P[2][14]*SF[7] + P[3][14]*SF[11] - P[12][14]*SF[15] + P[11][14]*SPP[10] - (P[10][14]*q0)/2) + SPP[5]*(P[1][15] + P[0][15]*SF[8] + P[2][15]*SF[7] + P[3][15]*SF[11] - P[12][15]*SF[15] + P[11][15]*SPP[10] - (P[10][15]*q0)/2);
    nextP[2][5] = P[2][5] + P[0][5]*SF[6] + P[1][5]*SF[10] + P[3][5]*SF[8] + P[12][5]*SF[14] - P[10][5]*SPP[10] - (P[11][5]*q0)*0.5f + SF[4]*(P[2][0] + P[0][0]*SF[6] + P[1][0]*SF[10] + P[3][0]*SF[8] + P[12][0]*SF[14] - P[10][0]*SPP[10] - (P[11][0]*q0)/2) + SF[3]*(P[2][2] + P[0][2]*SF[6] + P[1][2]*SF[10] + P[3][2]*SF[8] + P[12][2]*SF[14] - P[10][2]*SPP[10] - (P[11][2]*q0)/2) + SF[5]*(P[2][3] + P[0][3]*SF[6] + P[1][3]*SF[10] + P[3][3]*SF[8] + P[12][3]*SF[14] - P[10][3]*SPP[10] - (P[11][3]*q0)/2) - SPP[0]*(P[2][1] + P[0][1]*SF[6] + P[1][1]*SF[10] + P[3][1]*SF[8] + P[12][1]*SF[14] - P[10][1]*SPP[10] - (P[11][1]*q0)/2) - SPP[8]*(P[2][13] + P[0][13]*SF[6] + P[1][13]*SF[10] + P[3][13]*SF[8] + P[12][13]*SF[14] - P[10][13]*SPP[10] - (P[11][13]*q0)/2) + SPP[2]*(P[2][14] + P[0][14]*SF[6] + P[1][14]*SF[10] + P[3][14]*SF[8] + P[12][14]*SF[14] - P[10][14]*SPP[10] - (P[11][14]*q0)/2) + SPP[5]*(P[2][15] + P[0][15]*SF[6] + P[1][15]*SF[10] + P[3][15]*SF[8] + P[12][15]*SF[14] - P[10][15]*SPP[10] - (P[11][15]*q0)/2);
    nextP[3][5] = P[3][5] + P[0][5]*SF[7] + P[1][5]*SF[6] + P[2][5]*SF[9] + P[10][5]*SF[15] - P[11][5]*SF[14] - (P[12][5]*q0)*0.5f + SF[4]*(P[3][0] + P[0][0]*SF[7] + P[1][0]*SF[6] + P[2][0]*SF[9] + P[10][0]*SF[15] - P[11][0]*SF[14] - (P[12][0]*q0)/2) + SF[3]*(P[3][2] + P[0][2]*SF[7] + P[1][2]*SF[6] + P[2][2]*SF[9] + P[10][2]*SF[15] - P[11][2]*SF[14] - (P[12][2]*q0)/2) + SF[5]*(P[3][3] + P[0][3]*SF[7] + P[1][3]*SF[6] + P[2][3]*SF[9] + P[10][3]*SF[15] - P[11][3]*SF[14] - (P[12][3]*q0)/2) - SPP[0]*(P[3][1] + P[0][1]*SF[7] + P[1][1]*SF[6] + P[2][1]*SF[9] + P[10][1]*SF[15] - P[11][1]*SF[14] - (P[12][1]*q0)/2) - SPP[8]*(P[3][13] + P[0][13]*SF[7] + P[1][13]*SF[6] + P[2][13]*SF[9] + P[10][13]*SF[15] - P[11][13]*SF[14] - (P[12][13]*q0)/2) + SPP[2]*(P[3][14] + P[0][14]*SF[7] + P[1][14]*SF[6] + P[2][14]*SF[9] + P[10][14]*SF[15] - P[11][14]*SF[14] - (P[12][14]*q0)/2) + SPP[5]*(P[3][15] + P[0][15]*SF[7] + P[1][15]*SF[6] + P[2][15]*SF[9] + P[10][15]*SF[15] - P[11][15]*SF[14] - (P[12][15]*q0)/2);
    nextP[4][5] = P[4][5] + SQ[2] + P[0][5]*SF[5] + P[1][5]*SF[3] - P[3][5]*SF[4] + P[2][5]*SPP[0] + P[13][5]*SPP[3] + P[14][5]*SPP[6] - P[15][5]*SPP[9] + SF[4]*(P[4][0] + P[0][0]*SF[5] + P[1][0]*SF[3] - P[3][0]*SF[4] + P[2][0]*SPP[0] + P[13][0]*SPP[3] + P[14][0]*SPP[6] - P[15][0]*SPP[9]) + SF[3]*(P[4][2] + P[0][2]*SF[5] + P[1][2]*SF[3] - P[3][2]*SF[4] + P[2][2]*SPP[0] + P[13][2]*SPP[3] + P[14][2]*SPP[6] - P[15][2]*SPP[9]) + SF[5]*(P[4][3] + P[0][3]*SF[5] + P[1][3]*SF[3] - P[3][3]*SF[4] + P[2][3]*SPP[0] + P[13][3]*SPP[3] + P[14][3]*SPP[6] - P[15][3]*SPP[9]) - SPP[0]*(P[4][1] + P[0][1]*SF[5] + P[1][1]*SF[3] - P[3][1]*SF[4] + P[2][1]*SPP[0] + P[13][1]*SPP[3] + P[14][1]*SPP[6] - P[15][1]*SPP[9]) - SPP[8]*(P[4][13] + P[0][13]*SF[5] + P[1][13]*SF[3] - P[3][13]*SF[4] + P[2][13]*SPP[0] + P[13][13]*SPP[3] + P[14][13]*SPP[6] - P[15][13]*SPP[9]) + SPP[2]*(P[4][14] + P[0][14]*SF[5] + P[1][14]*SF[3] - P[3][14]*SF[4] + P[2][14]*SPP[0] + P[13][14]*SPP[3] + P[14][14]*SPP[6] - P[15][14]*SPP[9]) + SPP[5]*(P[4][15] + P[0][15]*SF[5] + P[1][15]*SF[3] - P[3][15]*SF[4] + P[2][15]*SPP[0] + P[13][15]*SPP[3] + P[14][15]*SPP[6] - P[15][15]*SPP[9]);
    nextP[5][5] = P[5][5] + P[0][5]*SF[4] + P[2][5]*SF[3] + P[3][5]*SF[5] - P[1][5]*SPP[0] - P[13][5]*SPP[8] + P[14][5]*SPP[2] + P[15][5]*SPP[5] + dvxVar*sq(SG[7] + 2.0f*q0*q3) + dvzVar*sq(SG[5] - 2.0f*q0*q1) + SF[4]*(P[5][0] + P[0][0]*SF[4] + P[2][0]*SF[3] + P[3][0]*SF[5] - P[1][0]*SPP[0] - P[13][0]*SPP[8] + P[14][0]*SPP[2] + P[15][0]*SPP[5]) + SF[3]*(P[5][2] + P[0][2]*SF[4] + P[2][2]*SF[3] + P[3][2]*SF[5] - P[1][2]*SPP[0] - P[13][2]*SPP[8] + P[14][2]*SPP[2] + P[15][2]*SPP[5]) + SF[5]*(P[5][3] + P[0][3]*SF[4] + P[2][3]*SF[3] + P[3][3]*SF[5] - P[1][3]*SPP[0] - P[13][3]*SPP[8] + P[14][3]*SPP[2] + P[15][3]*SPP[5]) - SPP[0]*(P[5][1] + P[0][1]*SF[4] + P[2][1]*SF[3] + P[3][1]*SF[5] - P[1][1]*SPP[0] - P[13][1]*SPP[8] + P[14][1]*SPP[2] + P[15][1]*SPP[5]) - SPP[8]*(P[5][13] + P[0][13]*SF[4] + P[2][13]*SF[3] + P[3][13]*SF[5] - P[1][13]*SPP[0] - P[13][13]*SPP[8] + P[14][13]*SPP[2] + P[15][13]*SPP[5]) + SPP[2]*(P[5][14] + P[0][14]*SF[4] + P[2][14]*SF[3] + P[3][14]*SF[5] - P[1][14]*SPP[0] - P[13][14]*SPP[8] + P[14][14]*SPP[2] + P[15][14]*SPP[5]) + SPP[5]*(P[5][15] + P[0][15]*SF[4] + P[2][15]*SF[3] + P[3][15]*SF[5] - P[1][15]*SPP[0] - P[13][15]*SPP[8] + P[14][15]*SPP[2] + P[15][15]*SPP[5]) + dvyVar*sq(SG[1] - SG[2] + SG[3] - SG[4]);
    nextP[0][6] = P[0][6] + P[1][6]*SF[9] + P[2][6]*SF[11] + P[3][6]*SF[10] + P[10][6]*SF[14] + P[11][6]*SF[15] + P[12][6]*SPP[10] + SF[4]*(P[0][1] + P[1][1]*SF[9] + P[2][1]*SF[11] + P[3][1]*SF[10] + P[10][1]*SF[14] + P[11][1]*SF[15] + P[12][1]*SPP[10]) - SF[5]*(P[0][2] + P[1][2]*SF[9] + P[2][2]*SF[11] + P[3][2]*SF[10] + P[10][2]*SF[14] + P[11][2]*SF[15] + P[12][2]*SPP[10]) + SF[3]*(P[0][3] + P[1][3]*SF[9] + P[2][3]*SF[11] + P[3][3]*SF[10] + P[10][3]*SF[14] + P[11][3]*SF[15] + P[12][3]*SPP[10]) + SPP[0]*(P[0][0] + P[1][0]*SF[9] + P[2][0]*SF[11] + P[3][0]*SF[10] + P[10][0]*SF[14] + P[11][0]*SF[15] + P[12][0]*SPP[10]) + SPP[4]*(P[0][13] + P[1][13]*SF[9] + P[2][13]*SF[11] + P[3][13]*SF[10] + P[10][13]*SF[14] + P[11][13]*SF[15] + P[12][13]*SPP[10]) - SPP[7]*(P[0][14] + P[1][14]*SF[9] + P[2][14]*SF[11] + P[3][14]*SF[10] + P[10][14]*SF[14] + P[11][14]*SF[15] + P[12][14]*SPP[10]) - SPP[1]*(P[0][15] + P[1][15]*SF[9] + P[2][15]*SF[11] + P[3][15]*SF[10] + P[10][15]*SF[14] + P[11][15]*SF[15] + P[12][15]*SPP[10]);
    nextP[1][6] = P[1][6] + P[0][6]*SF[8] + P[2][6]*SF[7] + P[3][6]*SF[11] - P[12][6]*SF[15] + P[11][6]*SPP[10] - (P[10][6]*q0)*0.5f + SF[4]*(P[1][1] + P[0][1]*SF[8] + P[2][1]*SF[7] + P[3][1]*SF[11] - P[12][1]*SF[15] + P[11][1]*SPP[10] - (P[10][1]*q0)/2) - SF[5]*(P[1][2] + P[0][2]*SF[8] + P[2][2]*SF[7] + P[3][2]*SF[11] - P[12][2]*SF[15] + P[11][2]*SPP[10] - (P[10][2]*q0)/2) + SF[3]*(P[1][3] + P[0][3]*SF[8] + P[2][3]*SF[7] + P[3][3]*SF[11] - P[12][3]*SF[15] + P[11][3]*SPP[10] - (P[10][3]*q0)/2) + SPP[0]*(P[1][0] + P[0][0]*SF[8] + P[2][0]*SF[7] + P[3][0]*SF[11] - P[12][0]*SF[15] + P[11][0]*SPP[10] - (P[10][0]*q0)/2) + SPP[4]*(P[1][13] + P[0][13]*SF[8] + P[2][13]*SF[7] + P[3][13]*SF[11] - P[12][13]*SF[15] + P[11][13]*SPP[10] - (P[10][13]*q0)/2) - SPP[7]*(P[1][14] + P[0][14]*SF[8] + P[2][14]*SF[7] + P[3][14]*SF[11] - P[12][14]*SF[15] + P[11][14]*SPP[10] - (P[10][14]*q0)/2) - SPP[1]*(P[1][15] + P[0][15]*SF[8] + P[2][15]*SF[7] + P[3][15]*SF[11] - P[12][15]*SF[15] + P[11][15]*SPP[10] - (P[10][15]*q0)/2);
    nextP[2][6] = P[2][6] + P[0][6]*SF[6] + P[1][6]*SF[10] + P[3][6]*SF[8] + P[12][6]*SF[14] - P[10][6]*SPP[10] - (P[11][6]*q0)*0.5f + SF[4]*(P[2][1] + P[0][1]*SF[6] + P[1][1]*SF[10] + P[3][1]*SF[8] + P[12][1]*SF[14] - P[10][1]*SPP[10] - (P[11][1]*q0)/2) - SF[5]*(P[2][2] + P[0][2]*SF[6] + P[1][2]*SF[10] + P[3][2]*SF[8] + P[12][2]*SF[14] - P[10][2]*SPP[10] - (P[11][2]*q0)/2) + SF[3]*(P[2][3] + P[0][3]*SF[6] + P[1][3]*SF[10] + P[3][3]*SF[8] + P[12][3]*SF[14] - P[10][3]*SPP[10] - (P[11][3]*q0)/2) + SPP[0]*(P[2][0] + P[0][0]*SF[6] + P[1][0]*SF[10] + P[3][0]*SF[8] + P[12][0]*SF[14] - P[10][0]*SPP[10] - (P[11][0]*q0)/2) + SPP[4]*(P[2][13] + P[0][13]*SF[6] + P[1][13]*SF[10] + P[3][13]*SF[8] + P[12][13]*SF[14] - P[10][13]*SPP[10] - (P[11][13]*q0)/2) - SPP[7]*(P[2][14] + P[0][14]*SF[6] + P[1][14]*SF[10] + P[3][14]*SF[8] + P[12][14]*SF[14] - P[10][14]*SPP[10] - (P[11][14]*q0)/2) - SPP[1]*(P[2][15] + P[0][15]*SF[6] + P[1][15]*SF[10] + P[3][15]*SF[8] + P[12][15]*SF[14] - P[10][15]*SPP[10] - (P[11][15]*q0)/2);
    nextP[3][6] = P[3][6] + P[0][6]*SF[7] + P[1][6]*SF[6] + P[2][6]*SF[9] + P[10][6]*SF[15] - P[11][6]*SF[14] - (P[12][6]*q0)*0.5f + SF[4]*(P[3][1] + P[0][1]*SF[7] + P[1][1]*SF[6] + P[2][1]*SF[9] + P[10][1]*SF[15] - P[11][1]*SF[14] - (P[12][1]*q0)/2) - SF[5]*(P[3][2] + P[0][2]*SF[7] + P[1][2]*SF[6] + P[2][2]*SF[9] + P[10][2]*SF[15] - P[11][2]*SF[14] - (P[12][2]*q0)/2) + SF[3]*(P[3][3] + P[0][3]*SF[7] + P[1][3]*SF[6] + P[2][3]*SF[9] + P[10][3]*SF[15] - P[11][3]*SF[14] - (P[12][3]*q0)/2) + SPP[0]*(P[3][0] + P[0][0]*SF[7] + P[1][0]*SF[6] + P[2][0]*SF[9] + P[10][0]*SF[15] - P[11][0]*SF[14] - (P[12][0]*q0)/2) + SPP[4]*(P[3][13] + P[0][13]*SF[7] + P[1][13]*SF[6] + P[2][13]*SF[9] + P[10][13]*SF[15] - P[11][13]*SF[14] - (P[12][13]*q0)/2) - SPP[7]*(P[3][14] + P[0][14]*SF[7] + P[1][14]*SF[6] + P[2][14]*SF[9] + P[10][14]*SF[15] - P[11][14]*SF[14] - (P[12][14]*q0)/2) - SPP[1]*(P[3][15] + P[0][15]*SF[7] + P[1][15]*SF[6] + P[2][15]*SF[9] + P[10][15]*SF[15] - P[11][15]*SF[14] - (P[12][15]*q0)/2);
    nextP[4][6] = P[4][6] + SQ[1] + P[0][6]*SF[5] + P[1][6]*SF[3] - P[3][6]*SF[4] + P[2][6]*SPP[0] + P[13][6]*SPP[3] + P[14][6]*SPP[6] - P[15][6]*SPP[9] + SF[4]*(P[4][1] + P[0][1]*SF[5] + P[1][1]*SF[3] - P[3][1]*SF[4] + P[2][1]*SPP[0] + P[13][1]*SPP[3] + P[14][1]*SPP[6] - P[15][1]*SPP[9]) - SF[5]*(P[4][2] + P[0][2]*SF[5] + P[1][2]*SF[3] - P[3][2]*SF[4] + P[2][2]*SPP[0] + P[13][2]*SPP[3] + P[14][2]*SPP[6] - P[15][2]*SPP[9]) + SF[3]*(P[4][3] + P[0][3]*SF[5] + P[1][3]*SF[3] - P[3][3]*SF[4] + P[2][3]*SPP[0] + P[13][3]*SPP[3] + P[14][3]*SPP[6] - P[15][3]*SPP[9]) + SPP[0]*(P[4][0] + P[0][0]*SF[5] + P[1][0]*SF[3] - P[3][0]*SF[4] + P[2][0]*SPP[0] + P[13][0]*SPP[3] + P[14][0]*SPP[6] - P[15][0]*SPP[9]) + SPP[4]*(P[4][13] + P[0][13]*SF[5] + P[1][13]*SF[3] - P[3][13]*SF[4] + P[2][13]*SPP[0] + P[13][13]*SPP[3] + P[14][13]*SPP[6] - P[15][13]*SPP[9]) - SPP[7]*(P[4][14] + P[0][14]*SF[5] + P[1][14]*SF[3] - P[3][14]*SF[4] + P[2][14]*SPP[0] + P[13][14]*SPP[3] + P[14][14]*SPP[6] - P[15][14]*SPP[9]) - SPP[1]*(P[4][15] + P[0][15]*SF[5] + P[1][15]*SF[3] - P[3][15]*SF[4] + P[2][15]*SPP[0] + P[13][15]*SPP[3] + P[14][15]*SPP[6] - P[15][15]*SPP[9]);
    nextP[5][6] = P[5][6] + SQ[0] + P[0][6]*SF[4] + P[2][6]*SF[3] + P[3][6]*SF[5] - P[1][6]*SPP[0] - P[13][6]*SPP[8] + P[14][6]*SPP[2] + P[15][6]*SPP[5] + SF[4]*(P[5][1] + P[0][1]*SF[4] + P[2][1]*SF[3] + P[3][1]*SF[5] - P[1][1]*SPP[0] - P[13][1]*SPP[8] + P[14][1]*SPP[2] + P[15][1]*SPP[5]) - SF[5]*(P[5][2] + P[0][2]*SF[4] + P[2][2]*SF[3] + P[3][2]*SF[5] - P[1][2]*SPP[0] - P[13][2]*SPP[8] + P[14][2]*SPP[2] + P[15][2]*SPP[5]) + SF[3]*(P[5][3] + P[0][3]*SF[4] + P[2][3]*SF[3] + P[3][3]*SF[5] - P[1][3]*SPP[0] - P[13][3]*SPP[8] + P[14][3]*SPP[2] + P[15][3]*SPP[5]) + SPP[0]*(P[5][0] + P[0][0]*SF[4] + P[2][0]*SF[3] + P[3][0]*SF[5] - P[1][0]*SPP[0] - P[13][0]*SPP[8] + P[14][0]*SPP[2] + P[15][0]*SPP[5]) + SPP[4]*(P[5][13] + P[0][13]*SF[4] + P[2][13]*SF[3] + P[3][13]*SF[5] - P[1][13]*SPP[0] - P[13][13]*SPP[8] + P[14][13]*SPP[2] + P[15][13]*SPP[5]) - SPP[7]*(P[5][14] + P[0][14]*SF[4] + P[2][14]*SF[3] + P[3][14]*SF[5] - P[1][14]*SPP[0] - P[13][14]*SPP[8] + P[14][14]*SPP[2] + P[15][14]*SPP[5]) - SPP[1]*(P[5][15] + P[0][15]*SF[4] + P[2][15]*SF[3] + P[3][15]*SF[5] - P[1][15]*SPP[0] - P[13][15]*SPP[8] + P[14][15]*SPP[2] + P[15][15]*SPP[5]);
    nextP[6][6] = P[6][6] + P[1][6]*SF[4] - P[2][6]*SF[5] + P[3][6]*SF[3] + P[0][6]*SPP[0] + P[13][6]*SPP[4] - P[14][6]*SPP[7] - P[15][6]*SPP[1] + dvxVar*sq(SG[6] - 2.0f*q0*q2) + dvyVar*sq(SG[5] + 2.0f*q0*q1) + SF[4]*(P[6][1] + P[1][1]*SF[4] - P[2][1]*SF[5] + P[3][1]*SF[3] + P[0][1]*SPP[0] + P[13][1]*SPP[4] - P[14][1]*SPP[7] - P[15][1]*SPP[1]) - SF[5]*(P[6][2] + P[1][2]*SF[4] - P[2][2]*SF[5] + P[3][2]*SF[3] + P[0][2]*SPP[0] + P[13][2]*SPP[4] - P[14][2]*SPP[7] - P[15][2]*SPP[1]) + SF[3]*(P[6][3] + P[1][3]*SF[4] - P[2][3]*SF[5] + P[3][3]*SF[3] + P[0][3]*SPP[0] + P[13][3]*SPP[4] - P[14][3]*SPP[7] - P[15][3]*SPP[1]) + SPP[0]*(P[6][0] + P[1][0]*SF[4] - P[2][0]*SF[5] + P[3][0]*SF[3] + P[0][0]*SPP[0] + P[13][0]*SPP[4] - P[14][0]*SPP[7] - P[15][0]*SPP[1]) + SPP[4]*(P[6][13] + P[1][13]*SF[4] - P[2][13]*SF[5] + P[3][13]*SF[3] + P[0][13]*SPP[0] + P[13][13]*SPP[4] - P[14][13]*SPP[7] - P[15][13]*SPP[1]) - SPP[7]*(P[6][14] + P[1][14]*SF[4] - P[2][14]*SF[5] + P[3][14]*SF[3] + P[0][14]*SPP[0] + P[13][14]*SPP[4] - P[14][14]*SPP[7] - P[15][14]*SPP[1]) - SPP[1]*(P[6][15] + P[1][15]*SF[4] - P[2][15]*SF[5] + P[3][15]*SF[3] + P[0][15]*SPP[0] + P[13][15]*SPP[4] - P[14][15]*SPP[7] - P[15][15]*SPP[1]) + dvzVar*sq(SG[1] - SG[2] - SG[3] + SG[4]);
    nextP[0][7] = P[0][7] + P[1][7]*SF[9] + P[2][7]*SF[11] + P[3][7]*SF[10] + P[10][7]*SF[14] + P[11][7]*SF[15] + P[12][7]*SPP[10] + dt*(P[0][4] + P[1][4]*SF[9] + P[2][4]*SF[11] + P[3][4]*SF[10] + P[10][4]*SF[14] + P[11][4]*SF[15] + P[12][4]*SPP[10]);
    nextP[1][7] = P[1][7] + P[0][7]*SF[8] + P[2][7]*SF[7] + P[3][7]*SF[11] - P[12][7]*SF[15] + P[11][7]*SPP[10] - (P[10][7]*q0)*0.5f + dt*(P[1][4] + P[0][4]*SF[8] + P[2][4]*SF[7] + P[3][4]*SF[11] - P[12][4]*SF[15] + P[11][4]*SPP[10] - (P[10][4]*q0)/2);
    nextP[2][7] = P[2][7] + P[0][7]*SF[6] + P[1][7]*SF[10] + P[3][7]*SF[8] + P[12][7]*SF[14] - P[10][7]*SPP[10] - (P[11][7]*q0)*0.5f + dt*(P[2][4] + P[0][4]*SF[6] + P[1][4]*SF[10] + P[3][4]*SF[8] + P[12][4]*SF[14] - P[10][4]*SPP[10] - (P[11][4]*q0)/2);
    nextP[3][7] = P[3][7] + P[0][7]*SF[7] + P[1][7]*SF[6] + P[2][7]*SF[9] + P[10][7]*SF[15] - P[11][7]*SF[14] - (P[12][7]*q0)*0.5f + dt*(P[3][4] + P[0][4]*SF[7] + P[1][4]*SF[6] + P[2][4]*SF[9] + P[10][4]*SF[15] - P[11][4]*SF[14] - (P[12][4]*q0)/2);
    nextP[4][7] = P[4][7] + P[0][7]*SF[5] + P[1][7]*SF[3] - P[3][7]*SF[4] + P[2][7]*SPP[0] + P[13][7]*SPP[3] + P[14][7]*SPP[6] - P[15][7]*SPP[9] + dt*(P[4][4] + P[0][4]*SF[5] + P[1][4]*SF[3] - P[3][4]*SF[4] + P[2][4]*SPP[0] + P[13][4]*SPP[3] + P[14][4]*SPP[6] - P[15][4]*SPP[9]);
    nextP[5][7] = P[5][7] + P[0][7]*SF[4] + P[2][7]*SF[3] + P[3][7]*SF[5] - P[1][7]*SPP[0] - P[13][7]*SPP[8] + P[14][7]*SPP[2] + P[15][7]*SPP[5] + dt*(P[5][4] + P[0][4]*SF[4] + P[2][4]*SF[3] + P[3][4]*SF[5] - P[1][4]*SPP[0] - P[13][4]*SPP[8] + P[14][4]*SPP[2] + P[15][4]*SPP[5]);
    nextP[6][7] = P[6][7] + P[1][7]*SF[4] - P[2][7]*SF[5] + P[3][7]*SF[3] + P[0][7]*SPP[0] + P[13][7]*SPP[4] - P[14][7]*SPP[7] - P[15][7]*SPP[1] + dt*(P[6][4] + P[1][4]*SF[4] - P[2][4]*SF[5] + P[3][4]*SF[3] + P[0][4]*SPP[0] + P[13][4]*SPP[4] - P[14][4]*SPP[7] - P[15][4]*SPP[1]);
    nextP[7][7] = P[7][7] + P[4][7]*dt + dt*(P[7][4] + P[4][4]*dt);
    nextP[0][8] = P[0][8] + P[1][8]*SF[9] + P[2][8]*SF[11] + P[3][8]*SF[10] + P[10][8]*SF[14] + P[11][8]*SF[15] + P[12][8]*SPP[10] + dt*(P[0][5] + P[1][5]*SF[9] + P[2][5]*SF[11] + P[3][5]*SF[10] + P[10][5]*SF[14] + P[11][5]*SF[15] + P[12][5]*SPP[10]);
    nextP[1][8] = P[1][8] + P[0][8]*SF[8] + P[2][8]*SF[7] + P[3][8]*SF[11] - P[12][8]*SF[15] + P[11][8]*SPP[10] - (P[10][8]*q0)*0.5f + dt*(P[1][5] + P[0][5]*SF[8] + P[2][5]*SF[7] + P[3][5]*SF[11] - P[12][5]*SF[15] + P[11][5]*SPP[10] - (P[10][5]*q0)/2);
    nextP[2][8] = P[2][8] + P[0][8]*SF[6] + P[1][8]*SF[10] + P[3][8]*SF[8] + P[12][8]*SF[14] - P[10][8]*SPP[10] - (P[11][8]*q0)*0.5f + dt*(P[2][5] + P[0][5]*SF[6] + P[1][5]*SF[10] + P[3][5]*SF[8] + P[12][5]*SF[14] - P[10][5]*SPP[10] - (P[11][5]*q0)/2);
    nextP[3][8] = P[3][8] + P[0][8]*SF[7] + P[1][8]*SF[6] + P[2][8]*SF[9] + P[10][8]*SF[15] - P[11][8]*SF[14] - (P[12][8]*q0)*0.5f + dt*(P[3][5] + P[0][5]*SF[7] + P[1][5]*SF[6] + P[2][5]*SF[9] + P[10][5]*SF[15] - P[11][5]*SF[14] - (P[12][5]*q0)/2);
    nextP[4][8] = P[4][8] + P[0][8]*SF[5] + P[1][8]*SF[3] - P[3][8]*SF[4] + P[2][8]*SPP[0] + P[13][8]*SPP[3] + P[14][8]*SPP[6] - P[15][8]*SPP[9] + dt*(P[4][5] + P[0][5]*SF[5] + P[1][5]*SF[3] - P[3][5]*SF[4] + P[2][5]*SPP[0] + P[13][5]*SPP[3] + P[14][5]*SPP[6] - P[15][5]*SPP[9]);
    nextP[5][8] = P[5][8] + P[0][8]*SF[4] + P[2][8]*SF[3] + P[3][8]*SF[5] - P[1][8]*SPP[0] - P[13][8]*SPP[8] + P[14][8]*SPP[2] + P[15][8]*SPP[5] + dt*(P[5][5] + P[0][5]*SF[4] + P[2][5]*SF[3] + P[3][5]*SF[5] - P[1][5]*SPP[0] - P[13][5]*SPP[8] + P[14][5]*SPP[2] + P[15][5]*SPP[5]);
    nextP[6][8] = P[6][8] + P[1][8]*SF[4] - P[2][8]*SF[5] + P[3][8]*SF[3] + P[0][8]*SPP[0] + P[13][8]*SPP[4] - P[14][8]*SPP[7] - P[15][8]*SPP[1] + dt*(P[6][5] + P[1][5]*SF[4] - P[2][5]*SF[5] + P[3][5]*SF[3] + P[0][5]*SPP[0] + P[13][5]*SPP[4] - P[14][5]*SPP[7] - P[15][5]*SPP[1]);
    nextP[7][8] = P[7][8] + P[4][8]*dt + dt*(P[7][5] + P[4][5]*dt);
    nextP[8][8] = P[8][8] + P[5][8]*dt + dt*(P[8][5] + P[5][5]*dt);
    nextP[0][9] = P[0][9] + P[1][9]*SF[9] + P[2][9]*SF[11] + P[3][9]*SF[10] + P[10][9]*SF[14] + P[11][9]*SF[15] + P[12][9]*SPP[10] + dt*(P[0][6] + P[1][6]*SF[9] + P[2][6]*SF[11] + P[3][6]*SF[10] + P[10][6]*SF[14] + P[11][6]*SF[15] + P[12][6]*SPP[10]);
    nextP[1][9] = P[1][9] + P[0][9]*SF[8] + P[2][9]*SF[7] + P[3][9]*SF[11] - P[12][9]*SF[15] + P[11][9]*SPP[10] - (P[10][9]*q0)*0.5f + dt*(P[1][6] + P[0][6]*SF[8] + P[2][6]*SF[7] + P[3][6]*SF[11] - P[12][6]*SF[15] + P[11][6]*SPP[10] - (P[10][6]*q0)/2);
    nextP[2][9] = P[2][9] + P[0][9]*SF[6] + P[1][9]*SF[10] + P[3][9]*SF[8] + P[12][9]*SF[14] - P[10][9]*SPP[10] - (P[11][9]*q0)*0.5f + dt*(P[2][6] + P[0][6]*SF[6] + P[1][6]*SF[10] + P[3][6]*SF[8] + P[12][6]*SF[14] - P[10][6]*SPP[10] - (P[11][6]*q0)/2);
    nextP[3][9] = P[3][9] + P[0][9]*SF[7] + P[1][9]*SF[6] + P[2][9]*SF[9] + P[10][9]*SF[15] - P[11][9]*SF[14] - (P[12][9]*q0)*0.5f + dt*(P[3][6] + P[0][6]*SF[7] + P[1][6]*SF[6] + P[2][6]*SF[9] + P[10][6]*SF[15] - P[11][6]*SF[14] - (P[12][6]*q0)/2);
    nextP[4][9] = P[4][9] + P[0][9]*SF[5] + P[1][9]*SF[3] - P[3][9]*SF[4] + P[2][9]*SPP[0] + P[13][9]*SPP[3] + P[14][9]*SPP[6] - P[15][9]*SPP[9] + dt*(P[4][6] + P[0][6]*SF[5] + P[1][6]*SF[3] - P[3][6]*SF[4] + P[2][6]*SPP[0] + P[13][6]*SPP[3] + P[14][6]*SPP[6] - P[15][6]*SPP[9]);
    nextP[5][9] = P[5][9] + P[0][9]*SF[4] + P[2][9]*SF[3] + P[3][9]*SF[5] - P[1][9]*SPP[0] - P[13][9]*SPP[8] + P[14][9]*SPP[2] + P[15][9]*SPP[5] + dt*(P[5][6] + P[0][6]*SF[4] + P[2][6]*SF[3] + P[3][6]*SF[5] - P[1][6]*SPP[0] - P[13][6]*SPP[8] + P[14][6]*SPP[2] + P[15][6]*SPP[5]);
    nextP[6][9] = P[6][9] + P[1][9]*SF[4] - P[2][9]*SF[5] + P[3][9]*SF[3] + P[0][9]*SPP[0] + P[13][9]*SPP[4] - P[14][9]*SPP[7] - P[15][9]*SPP[1] + dt*(P[6][6] + P[1][6]*SF[4] - P[2][6]*SF[5] + P[3][6]*SF[3] + P[0][6]*SPP[0] + P[13][6]*SPP[4] - P[14][6]*SPP[7] - P[15][6]*SPP[1]);
    nextP[7][9] = P[7][9] + P[4][9]*dt + dt*(P[7][6] + P[4][6]*dt);
    nextP[8][9] = P[8][9] + P[5][9]*dt + dt*(P[8][6] + P[5][6]*dt);
    nextP[9][9] = P[9][9] + P[6][9]*dt + dt*(P[9][6] + P[6][6]*dt);
    nextP[0][10] = P[0][10] + P[1][10]*SF[9] + P[2][10]*SF[11] + P[3][10]*SF[10] + P[10][10]*SF[14] + P[11][10]*SF[15] + P[12][10]*SPP[10];
    nextP[1][10] = P[1][10] + P[0][10]*SF[8] + P[2][10]*SF[7] + P[3][10]*SF[11] - P[12][10]*SF[15] + P[11][10]*SPP[10] - (P[10][10]*q0)*0.5f;
    nextP[2][10] = P[2][10] + P[0][10]*SF[6] + P[1][10]*SF[10] + P[3][10]*SF[8] + P[12][10]*SF[14] - P[10][10]*SPP[10] - (P[11][10]*q0)*0.5f;
    nextP[3][10] = P[3][10] + P[0][10]*SF[7] + P[1][10]*SF[6] + P[2][10]*SF[9] + P[10][10]*SF[15] - P[11][10]*SF[14] - (P[12][10]*q0)*0.5f;
    nextP[4][10] = P[4][10] + P[0][10]*SF[5] + P[1][10]*SF[3] - P[3][10]*SF[4] + P[2][10]*SPP[0] + P[13][10]*SPP[3] + P[14][10]*SPP[6] - P[15][10]*SPP[9];
    nextP[5][10] = P[5][10] + P[0][10]*SF[4] + P[2][10]*SF[3] + P[3][10]*SF[5] - P[1][10]*SPP[0] - P[13][10]*SPP[8] + P[14][10]*SPP[2] + P[15][10]*SPP[5];
    nextP[6][10] = P[6][10] + P[1][10]*SF[4] - P[2][10]*SF[5] + P[3][10]*SF[3] + P[0][10]*SPP[0] + P[13][10]*SPP[4] - P[14][10]*SPP[7] - P[15][10]*SPP[1];
    nextP[7][10] = P[7][10] + P[4][10]*dt;
    nextP[8][10] = P[8][10] + P[5][10]*dt;
    nextP[9][10] = P[9][10] + P[6][10]*dt;
    nextP[10][10] = P[10][10];
    nextP[0][11] = P[0][11] + P[1][11]*SF[9] + P[2][11]*SF[11] + P[3][11]*SF[10] + P[10][11]*SF[14] + P[11][11]*SF[15] + P[12][11]*SPP[10];
    nextP[1][11] = P[1][11] + P[0][11]*SF[8] + P[2][11]*SF[7] + P[3][11]*SF[11] - P[12][11]*SF[15] + P[11][11]*SPP[10] - (P[10][11]*q0)*0.5f;
    nextP[2][11] = P[2][11] + P[0][11]*SF[6] + P[1][11]*SF[10] + P[3][11]*SF[8] + P[12][11]*SF[14] - P[10][11]*SPP[10] - (P[11][11]*q0)*0.5f;
    nextP[3][11] = P[3][11] + P[0][11]*SF[7] + P[1][11]*SF[6] + P[2][11]*SF[9] + P[10][11]*SF[15] - P[11][11]*SF[14] - (P[12][11]*q0)*0.5f;
    nextP[4][11] = P[4][11] + P[0][11]*SF[5] + P[1][11]*SF[3] - P[3][11]*SF[4] + P[2][11]*SPP[0] + P[13][11]*SPP[3] + P[14][11]*SPP[6] - P[15][11]*SPP[9];
    nextP[5][11] = P[5][11] + P[0][11]*SF[4] + P[2][11]*SF[3] + P[3][11]*SF[5] - P[1][11]*SPP[0] - P[13][11]*SPP[8] + P[14][11]*SPP[2] + P[15][11]*SPP[5];
    nextP[6][11] = P[6][11] + P[1][11]*SF[4] - P[2][11]*SF[5] + P[3][11]*SF[3] + P[0][11]*SPP[0] + P[13][11]*SPP[4] - P[14][11]*SPP[7] - P[15][11]*SPP[1];
    nextP[7][11] = P[7][11] + P[4][11]*dt;
    nextP[8][11] = P[8][11] + P[5][11]*dt;
    nextP[9][11] = P[9][11] + P[6][11]*dt;
    nextP[10][11] = P[10][11];
    nextP[11][11] = P[11][11];
    nextP[0][12] = P[0][12] + P[1][12]*SF[9] + P[2][12]*SF[11] + P[3][12]*SF[10] + P[10][12]*SF[14] + P[11][12]*SF[15] + P[12][12]*SPP[10];
    nextP[1][12] = P[1][12] + P[0][12]*SF[8] + P[2][12]*SF[7] + P[3][12]*SF[11] - P[12][12]*SF[15] + P[11][12]*SPP[10] - (P[10][12]*q0)*0.5f;
    nextP[2][12] = P[2][12] + P[0][12]*SF[6] + P[1][12]*SF[10] + P[3][12]*SF[8] + P[12][12]*SF[14] - P[10][12]*SPP[10] - (P[11][12]*q0)*0.5f;
    nextP[3][12] = P[3][12] + P[0][12]*SF[7] + P[1][12]*SF[6] + P[2][12]*SF[9] + P[10][12]*SF[15] - P[11][12]*SF[14] - (P[12][12]*q0)*0.5f;
    nextP[4][12] = P[4][12] + P[0][12]*SF[5] + P[1][12]*SF[3] - P[3][12]*SF[4] + P[2][12]*SPP[0] + P[13][12]*SPP[3] + P[14][12]*SPP[6] - P[15][12]*SPP[9];
    nextP[5][12] = P[5][12] + P[0][12]*SF[4] + P[2][12]*SF[3] + P[3][12]*SF[5] - P[1][12]*SPP[0] - P[13][12]*SPP[8] + P[14][12]*SPP[2] + P[15][12]*SPP[5];
    nextP[6][12] = P[6][12] + P[1][12]*SF[4] - P[2][12]*SF[5] + P[3][12]*SF[3] + P[0][12]*SPP[0] + P[13][12]*SPP[4] - P[14][12]*SPP[7] - P[15][12]*SPP[1];
    nextP[7][12] = P[7][12] + P[4][12]*dt;
    nextP[8][12] = P[8][12] + P[5][12]*dt;
    nextP[9][12] = P[9][12] + P[6][12]*dt;
    nextP[10][12] = P[10][12];
    nextP[11][12] = P[11][12];
    nextP[12][12] = P[12][12];
    nextP[0][13] = P[0][13] + P[1][13]*SF[9] + P[2][13]*SF[11] + P[3][13]*SF[10] + P[10][13]*SF[14] + P[11][13]*SF[15] + P[12][13]*SPP[10];
    nextP[1][13] = P[1][13] + P[0][13]*SF[8] + P[2][13]*SF[7] + P[3][13]*SF[11] - P[12][13]*SF[15] + P[11][13]*SPP[10] - (P[10][13]*q0)*0.5f;
    nextP[2][13] = P[2][13] + P[0][13]*SF[6] + P[1][13]*SF[10] + P[3][13]*SF[8] + P[12][13]*SF[14] - P[10][13]*SPP[10] - (P[11][13]*q0)*0.5f;
    nextP[3][13] = P[3][13] + P[0][13]*SF[7] + P[1][13]*SF[6] + P[2][13]*SF[9] + P[10][13]*SF[15] - P[11][13]*SF[14] - (P[12][13]*q0)*0.5f;
    nextP[4][13] = P[4][13] + P[0][13]*SF[5] + P[1][13]*SF[3] - P[3][13]*SF[4] + P[2][13]*SPP[0] + P[13][13]*SPP[3] + P[14][13]*SPP[6] - P[15][13]*SPP[9];
    nextP[5][13] = P[5][13] + P[0][13]*SF[4] + P[2][13]*SF[3] + P[3][13]*SF[5] - P[1][13]*SPP[0] - P[13][13]*SPP[8] + P[14][13]*SPP[2] + P[15][13]*SPP[5];
    nextP[6][13] = P[6][13] + P[1][13]*SF[4] - P[2][13]*SF[5] + P[3][13]*SF[3] + P[0][13]*SPP[0] + P[13][13]*SPP[4] - P[14][13]*SPP[7] - P[15][13]*SPP[1];
    nextP[7][13] = P[7][13] + P[4][13]*dt;
    nextP[8][13] = P[8][13] + P[5][13]*dt;
    nextP[9][13] = P[9][13] + P[6][13]*dt;
    nextP[10][13] = P[10][13];
    nextP[11][13] = P[11][13];
    nextP[12][13] = P[12][13];
    nextP[13][13] = P[13][13];
    nextP[0][14] = P[0][14] + P[1][14]*SF[9] + P[2][14]*SF[11] + P[3][14]*SF[10] + P[10][14]*SF[14] + P[11][14]*SF[15] + P[12][14]*SPP[10];
    nextP[1][14] = P[1][14] + P[0][14]*SF[8] + P[2][14]*SF[7] + P[3][14]*SF[11] - P[12][14]*SF[15] + P[11][14]*SPP[10] - (P[10][14]*q0)*0.5f;
    nextP[2][14] = P[2][14] + P[0][14]*SF[6] + P[1][14]*SF[10] + P[3][14]*SF[8] + P[12][14]*SF[14] - P[10][14]*SPP[10] - (P[11][14]*q0)*0.5f;
    nextP[3][14] = P[3][14] + P[0][14]*SF[7] + P[1][14]*SF[6] + P[2][14]*SF[9] + P[10][14]*SF[15] - P[11][14]*SF[14] - (P[12][14]*q0)*0.5f;
    nextP[4][14] = P[4][14] + P[0][14]*SF[5] + P[1][14]*SF[3] - P[3][14]*SF[4] + P[2][14]*SPP[0] + P[13][14]*SPP[3] + P[14][14]*SPP[6] - P[15][14]*SPP[9];
    nextP[5][14] = P[5][14] + P[0][14]*SF[4] + P[2][14]*SF[3] + P[3][14]*SF[5] - P[1][14]*SPP[0] - P[13][14]*SPP[8] + P[14][14]*SPP[2] + P[15][14]*SPP[5];
    nextP[6][14] = P[6][14] + P[1][14]*SF[4] - P[2][14]*SF[5] + P[3][14]*SF[3] + P[0][14]*SPP[0] + P[13][14]*SPP[4] - P[14][14]*SPP[7] - P[15][14]*SPP[1];
    nextP[7][14] = P[7][14] + P[4][14]*dt;
    nextP[8][14] = P[8][14] + P[5][14]*dt;
    nextP[9][14] = P[9][14] + P[6][14]*dt;
    nextP[10][14] = P[10][14];
    nextP[11][14] = P[11][14];
    nextP[12][14] = P[12][14];
    nextP[13][14] = P[13][14];
    nextP[14][14] = P[14][14];
    nextP[0][15] = P[0][15] + P[1][15]*SF[9] + P[2][15]*SF[11] + P[3][15]*SF[10] + P[10][15]*SF[14] + P[11][15]*SF[15] + P[12][15]*SPP[10];
    nextP[1][15] = P[1][15] + P[0][15]*SF[8] + P[2][15]*SF[7] + P[3][15]*SF[11] - P[12][15]*SF[15] + P[11][15]*SPP[10] - (P[10][15]*q0)*0.5f;
    nextP[2][15] = P[2][15] + P[0][15]*SF[6] + P[1][15]*SF[10] + P[3][15]*SF[8] + P[12][15]*SF[14] - P[10][15]*SPP[10] - (P[11][15]*q0)*0.5f;
    nextP[3][15] = P[3][15] + P[0][15]*SF[7] + P[1][15]*SF[6] + P[2][15]*SF[9] + P[10][15]*SF[15] - P[11][15]*SF[14] - (P[12][15]*q0)*0.5f;
    nextP[4][15] = P[4][15] + P[0][15]*SF[5] + P[1][15]*SF[3] - P[3][15]*SF[4] + P[2][15]*SPP[0] + P[13][15]*SPP[3] + P[14][15]*SPP[6] - P[15][15]*SPP[9];
    nextP[5][15] = P[5][15] + P[0][15]*SF[4] + P[2][15]*SF[3] + P[3][15]*SF[5] - P[1][15]*SPP[0] - P[13][15]*SPP[8] + P[14][15]*SPP[2] + P[15][15]*SPP[5];
    nextP[6][15] = P[6][15] + P[1][15]*SF[4] - P[2][15]*SF[5] + P[3][15]*SF[3] + P[0][15]*SPP[0] + P[13][15]*SPP[4] - P[14][15]*SPP[7] - P[15][15]*SPP[1];
    nextP[7][15] = P[7][15] + P[4][15]*dt;
    nextP[8][15] = P[8][15] + P[5][15]*dt;
    nextP[9][15] = P[9][15] + P[6][15]*dt;
    nextP[10][15] = P[10][15];
    nextP[11][15] = P[11][15];
    nextP[12][15] = P[12][15];
    nextP[13][15] = P[13][15];
    nextP[14][15] = P[14][15];
    nextP[15][15] = P[15][15];

    if (stateIndexLim > 15) {
        nextP[0][16] = P[0][16] + P[1][16]*SF[9] + P[2][16]*SF[11] + P[3][16]*SF[10] + P[10][16]*SF[14] + P[11][16]*SF[15] + P[12][16]*SPP[10];
        nextP[1][16] = P[1][16] + P[0][16]*SF[8] + P[2][16]*SF[7] + P[3][16]*SF[11] - P[12][16]*SF[15] + P[11][16]*SPP[10] - (P[10][16]*q0)*0.5f;
        nextP[2][16] = P[2][16] + P[0][16]*SF[6] + P[1][16]*SF[10] + P[3][16]*SF[8] + P[12][16]*SF[14] - P[10][16]*SPP[10] - (P[11][16]*q0)*0.5f;
        nextP[3][16] = P[3][16] + P[0][16]*SF[7] + P[1][16]*SF[6] + P[2][16]*SF[9] + P[10][16]*SF[15] - P[11][16]*SF[14] - (P[12][16]*q0)*0.5f;
        nextP[4][16] = P[4][16] + P[0][16]*SF[5] + P[1][16]*SF[3] - P[3][16]*SF[4] + P[2][16]*SPP[0] + P[13][16]*SPP[3] + P[14][16]*SPP[6] - P[15][16]*SPP[9];
        nextP[5][16] = P[5][16] + P[0][16]*SF[4] + P[2][16]*SF[3] + P[3][16]*SF[5] - P[1][16]*SPP[0] - P[13][16]*SPP[8] + P[14][16]*SPP[2] + P[15][16]*SPP[5];
        nextP[6][16] = P[6][16] + P[1][16]*SF[4] - P[2][16]*SF[5] + P[3][16]*SF[3] + P[0][16]*SPP[0] + P[13][16]*SPP[4] - P[14][16]*SPP[7] - P[15][16]*SPP[1];
        nextP[7][16] = P[7][16] + P[4][16]*dt;
        nextP[8][16] = P[8][16] + P[5][16]*dt;
        nextP[9][16] = P[9][16] + P[6][16]*dt;
        nextP[10][16] = P[10][16];
        nextP[11][16] = P[11][16];
        nextP[12][16] = P[12][16];
        nextP[13][16] = P[13][16];
        nextP[14][16] = P[14][16];
        nextP[15][16] = P[15][16];
        nextP[16][16] = P[16][16];
        nextP[0][17] = P[0][17] + P[1][17]*SF[9] + P[2][17]*SF[11] + P[3][17]*SF[10] + P[10][17]*SF[14] + P[11][17]*SF[15] + P[12][17]*SPP[10];
        nextP[1][17] = P[1][17] + P[0][17]*SF[8] + P[2][17]*SF[7] + P[3][17]*SF[11] - P[12][17]*SF[15] + P[11][17]*SPP[10] - (P[10][17]*q0)*0.5f;
        nextP[2][17] = P[2][17] + P[0][17]*SF[6] + P[1][17]*SF[10] + P[3][17]*SF[8] + P[12][17]*SF[14] - P[10][17]*SPP[10] - (P[11][17]*q0)*0.5f;
        nextP[3][17] = P[3][17] + P[0][17]*SF[7] + P[1][17]*SF[6] + P[2][17]*SF[9] + P[10][17]*SF[15] - P[11][17]*SF[14] - (P[12][17]*q0)*0.5f;
        nextP[4][17] = P[4][17] + P[0][17]*SF[5] + P[1][17]*SF[3] - P[3][17]*SF[4] + P[2][17]*SPP[0] + P[13][17]*SPP[3] + P[14][17]*SPP[6] - P[15][17]*SPP[9];
        nextP[5][17] = P[5][17] + P[0][17]*SF[4] + P[2][17]*SF[3] + P[3][17]*SF[5] - P[1][17]*SPP[0] - P[13][17]*SPP[8] + P[14][17]*SPP[2] + P[15][17]*SPP[5];
        nextP[6][17] = P[6][17] + P[1][17]*SF[4] - P[2][17]*SF[5] + P[3][17]*SF[3] + P[0][17]*SPP[0] + P[13][17]*SPP[4] - P[14][17]*SPP[7] - P[15][17]*SPP[1];
        nextP[7][17] = P[7][17] + P[4][17]*dt;
        nextP[8][17] = P[8][17] + P[5][17]*dt;
        nextP[9][17] = P[9][17] + P[6][17]*dt;
        nextP[10][17] = P[10][17];
        nextP[11][17] = P[11][17];
        nextP[12][17] = P[12][17];
        nextP[13][17] = P[13][17];
        nextP[14][17] = P[14][17];
        nextP[15][17] = P[15][17];
        nextP[16][17] = P[16][17];
        nextP[17][17] = P[17][17];
        nextP[0][18] = P[0][18] + P[1][18]*SF[9] + P[2][18]*SF[11] + P[3][18]*SF[10] + P[10][18]*SF[14] + P[11][18]*SF[15] + P[12][18]*SPP[10];
        nextP[1][18] = P[1][18] + P[0][18]*SF[8] + P[2][18]*SF[7] + P[3][18]*SF[11] - P[12][18]*SF[15] + P[11][18]*SPP[10] - (P[10][18]*q0)*0.5f;
        nextP[2][18] = P[2][18] + P[0][18]*SF[6] + P[1][18]*SF[10] + P[3][18]*SF[8] + P[12][18]*SF[14] - P[10][18]*SPP[10] - (P[11][18]*q0)*0.5f;
        nextP[3][18] = P[3][18] + P[0][18]*SF[7] + P[1][18]*SF[6] + P[2][18]*SF[9] + P[10][18]*SF[15] - P[11][18]*SF[14] - (P[12][18]*q0)*0.5f;
        nextP[4][18] = P[4][18] + P[0][18]*SF[5] + P[1][18]*SF[3] - P[3][18]*SF[4] + P[2][18]*SPP[0] + P[13][18]*SPP[3] + P[14][18]*SPP[6] - P[15][18]*SPP[9];
        nextP[5][18] = P[5][18] + P[0][18]*SF[4] + P[2][18]*SF[3] + P[3][18]*SF[5] - P[1][18]*SPP[0] - P[13][18]*SPP[8] + P[14][18]*SPP[2] + P[15][18]*SPP[5];
        nextP[6][18] = P[6][18] + P[1][18]*SF[4] - P[2][18]*SF[5] + P[3][18]*SF[3] + P[0][18]*SPP[0] + P[13][18]*SPP[4] - P[14][18]*SPP[7] - P[15][18]*SPP[1];
        nextP[7][18] = P[7][18] + P[4][18]*dt;
        nextP[8][18] = P[8][18] + P[5][18]*dt;
        nextP[9][18] = P[9][18] + P[6][18]*dt;
        nextP[10][18] = P[10][18];
        nextP[11][18] = P[11][18];
        nextP[12][18] = P[12][18];
        nextP[13][18] = P[13][18];
        nextP[14][18] = P[14][18];
        nextP[15][18] = P[15][18];
        nextP[16][18] = P[16][18];
        nextP[17][18] = P[17][18];
        nextP[18][18] = P[18][18];
        nextP[0][19] = P[0][19] + P[1][19]*SF[9] + P[2][19]*SF[11] + P[3][19]*SF[10] + P[10][19]*SF[14] + P[11][19]*SF[15] + P[12][19]*SPP[10];
        nextP[1][19] = P[1][19] + P[0][19]*SF[8] + P[2][19]*SF[7] + P[3][19]*SF[11] - P[12][19]*SF[15] + P[11][19]*SPP[10] - (P[10][19]*q0)*0.5f;
        nextP[2][19] = P[2][19] + P[0][19]*SF[6] + P[1][19]*SF[10] + P[3][19]*SF[8] + P[12][19]*SF[14] - P[10][19]*SPP[10] - (P[11][19]*q0)*0.5f;
        nextP[3][19] = P[3][19] + P[0][19]*SF[7] + P[1][19]*SF[6] + P[2][19]*SF[9] + P[10][19]*SF[15] - P[11][19]*SF[14] - (P[12][19]*q0)*0.5f;
        nextP[4][19] = P[4][19] + P[0][19]*SF[5] + P[1][19]*SF[3] - P[3][19]*SF[4] + P[2][19]*SPP[0] + P[13][19]*SPP[3] + P[14][19]*SPP[6] - P[15][19]*SPP[9];
        nextP[5][19] = P[5][19] + P[0][19]*SF[4] + P[2][19]*SF[3] + P[3][19]*SF[5] - P[1][19]*SPP[0] - P[13][19]*SPP[8] + P[14][19]*SPP[2] + P[15][19]*SPP[5];
        nextP[6][19] = P[6][19] + P[1][19]*SF[4] - P[2][19]*SF[5] + P[3][19]*SF[3] + P[0][19]*SPP[0] + P[13][19]*SPP[4] - P[14][19]*SPP[7] - P[15][19]*SPP[1];
        nextP[7][19] = P[7][19] + P[4][19]*dt;
        nextP[8][19] = P[8][19] + P[5][19]*dt;
        nextP[9][19] = P[9][19] + P[6][19]*dt;
        nextP[10][19] = P[10][19];
        nextP[11][19] = P[11][19];
        nextP[12][19] = P[12][19];
        nextP[13][19] = P[13][19];
        nextP[14][19] = P[14][19];
        nextP[15][19] = P[15][19];
        nextP[16][19] = P[16][19];
        nextP[17][19] = P[17][19];
        nextP[18][19] = P[18][19];
        nextP[19][19] = P[19][19];
        nextP[0][20] = P[0][20] + P[1][20]*SF[9] + P[2][20]*SF[11] + P[3][20]*SF[10] + P[10][20]*SF[14] + P[11][20]*SF[15] + P[12][20]*SPP[10];
        nextP[1][20] = P[1][20] + P[0][20]*SF[8] + P[2][20]*SF[7] + P[3][20]*SF[11] - P[12][20]*SF[15] + P[11][20]*SPP[10] - (P[10][20]*q0)*0.5f;
        nextP[2][20] = P[2][20] + P[0][20]*SF[6] + P[1][20]*SF[10] + P[3][20]*SF[8] + P[12][20]*SF[14] - P[10][20]*SPP[10] - (P[11][20]*q0)*0.5f;
        nextP[3][20] = P[3][20] + P[0][20]*SF[7] + P[1][20]*SF[6] + P[2][20]*SF[9] + P[10][20]*SF[15] - P[11][20]*SF[14] - (P[12][20]*q0)*0.5f;
        nextP[4][20] = P[4][20] + P[0][20]*SF[5] + P[1][20]*SF[3] - P[3][20]*SF[4] + P[2][20]*SPP[0] + P[13][20]*SPP[3] + P[14][20]*SPP[6] - P[15][20]*SPP[9];
        nextP[5][20] = P[5][20] + P[0][20]*SF[4] + P[2][20]*SF[3] + P[3][20]*SF[5] - P[1][20]*SPP[0] - P[13][20]*SPP[8] + P[14][20]*SPP[2] + P[15][20]*SPP[5];
        nextP[6][20] = P[6][20] + P[1][20]*SF[4] - P[2][20]*SF[5] + P[3][20]*SF[3] + P[0][20]*SPP[0] + P[13][20]*SPP[4] - P[14][20]*SPP[7] - P[15][20]*SPP[1];
        nextP[7][20] = P[7][20] + P[4][20]*dt;
        nextP[8][20] = P[8][20] + P[5][20]*dt;
        nextP[9][20] = P[9][20] + P[6][20]*dt;
        nextP[10][20] = P[10][20];
        nextP[11][20] = P[11][20];
        nextP[12][20] = P[12][20];
        nextP[13][20] = P[13][20];
        nextP[14][20] = P[14][20];
        nextP[15][20] = P[15][20];
        nextP[16][20] = P[16][20];
        nextP[17][20] = P[17][20];
        nextP[18][20] = P[18][20];
        nextP[19][20] = P[19][20];
        nextP[20][20] = P[20][20];
        nextP[0][21] = P[0][21] + P[1][21]*SF[9] + P[2][21]*SF[11] + P[3][21]*SF[10] + P[10][21]*SF[14] + P[11][21]*SF[15] + P[12][21]*SPP[10];
        nextP[1][21] = P[1][21] + P[0][21]*SF[8] + P[2][21]*SF[7] + P[3][21]*SF[11] - P[12][21]*SF[15] + P[11][21]*SPP[10] - (P[10][21]*q0)*0.5f;
        nextP[2][21] = P[2][21] + P[0][21]*SF[6] + P[1][21]*SF[10] + P[3][21]*SF[8] + P[12][21]*SF[14] - P[10][21]*SPP[10] - (P[11][21]*q0)*0.5f;
        nextP[3][21] = P[3][21] + P[0][21]*SF[7] + P[1][21]*SF[6] + P[2][21]*SF[9] + P[10][21]*SF[15] - P[11][21]*SF[14] - (P[12][21]*q0)*0.5f;
        nextP[4][21] = P[4][21] + P[0][21]*SF[5] + P[1][21]*SF[3] - P[3][21]*SF[4] + P[2][21]*SPP[0] + P[13][21]*SPP[3] + P[14][21]*SPP[6] - P[15][21]*SPP[9];
        nextP[5][21] = P[5][21] + P[0][21]*SF[4] + P[2][21]*SF[3] + P[3][21]*SF[5] - P[1][21]*SPP[0] - P[13][21]*SPP[8] + P[14][21]*SPP[2] + P[15][21]*SPP[5];
        nextP[6][21] = P[6][21] + P[1][21]*SF[4] - P[2][21]*SF[5] + P[3][21]*SF[3] + P[0][21]*SPP[0] + P[13][21]*SPP[4] - P[14][21]*SPP[7] - P[15][21]*SPP[1];
        nextP[7][21] = P[7][21] + P[4][21]*dt;
        nextP[8][21] = P[8][21] + P[5][21]*dt;
        nextP[9][21] = P[9][21] + P[6][21]*dt;
        nextP[10][21] = P[10][21];
        nextP[11][21] = P[11][21];
        nextP[12][21] = P[12][21];
        nextP[13][21] = P[13][21];
        nextP[14][21] = P[14][21];
        nextP[15][21] = P[15][21];
        nextP[16][21] = P[16][21];
        nextP[17][21] = P[17][21];
        nextP[18][21] = P[18][21];
        nextP[19][21] = P[19][21];
        nextP[20][21] = P[20][21];
        nextP[21][21] = P[21][21];

        if (stateIndexLim > 21) {
            nextP[0][22] = P[0][22] + P[1][22]*SF[9] + P[2][22]*SF[11] + P[3][22]*SF[10] + P[10][22]*SF[14] + P[11][22]*SF[15] + P[12][22]*SPP[10];
            nextP[1][22] = P[1][22] + P[0][22]*SF[8] + P[2][22]*SF[7] + P[3][22]*SF[11] - P[12][22]*SF[15] + P[11][22]*SPP[10] - (P[10][22]*q0)*0.5f;
            nextP[2][22] = P[2][22] + P[0][22]*SF[6] + P[1][22]*SF[10] + P[3][22]*SF[8] + P[12][22]*SF[14] - P[10][22]*SPP[10] - (P[11][22]*q0)*0.5f;
            nextP[3][22] = P[3][22] + P[0][22]*SF[7] + P[1][22]*SF[6] + P[2][22]*SF[9] + P[10][22]*SF[15] - P[11][22]*SF[14] - (P[12][22]*q0)*0.5f;
            nextP[4][22] = P[4][22] + P[0][22]*SF[5] + P[1][22]*SF[3] - P[3][22]*SF[4] + P[2][22]*SPP[0] + P[13][22]*SPP[3] + P[14][22]*SPP[6] - P[15][22]*SPP[9];
            nextP[5][22] = P[5][22] + P[0][22]*SF[4] + P[2][22]*SF[3] + P[3][22]*SF[5] - P[1][22]*SPP[0] - P[13][22]*SPP[8] + P[14][22]*SPP[2] + P[15][22]*SPP[5];
            nextP[6][22] = P[6][22] + P[1][22]*SF[4] - P[2][22]*SF[5] + P[3][22]*SF[3] + P[0][22]*SPP[0] + P[13][22]*SPP[4] - P[14][22]*SPP[7] - P[15][22]*SPP[1];
            nextP[7][22] = P[7][22] + P[4][22]*dt;
            nextP[8][22] = P[8][22] + P[5][22]*dt;
            nextP[9][22] = P[9][22] + P[6][22]*dt;
            nextP[10][22] = P[10][22];
            nextP[11][22] = P[11][22];
            nextP[12][22] = P[12][22];
            nextP[13][22] = P[13][22];
            nextP[14][22] = P[14][22];
            nextP[15][22] = P[15][22];
            nextP[16][22] = P[16][22];
            nextP[17][22] = P[17][22];
            nextP[18][22] = P[18][22];
            nextP[19][22] = P[19][22];
            nextP[20][22] = P[20][22];
            nextP[21][22] = P[21][22];
            nextP[22][22] = P[22][22];
            nextP[0][23] = P[0][23] + P[1][23]*SF[9] + P[2][23]*SF[11] + P[3][23]*SF[10] + P[10][23]*SF[14] + P[11][23]*SF[15] + P[12][23]*SPP[10];
            nextP[1][23] = P[1][23] + P[0][23]*SF[8] + P[2][23]*SF[7] + P[3][23]*SF[11] - P[12][23]*SF[15] + P[11][23]*SPP[10] - (P[10][23]*q0)*0.5f;
            nextP[2][23] = P[2][23] + P[0][23]*SF[6] + P[1][23]*SF[10] + P[3][23]*SF[8] + P[12][23]*SF[14] - P[10][23]*SPP[10] - (P[11][23]*q0)*0.5f;
            nextP[3][23] = P[3][23] + P[0][23]*SF[7] + P[1][23]*SF[6] + P[2][23]*SF[9] + P[10][23]*SF[15] - P[11][23]*SF[14] - (P[12][23]*q0)*0.5f;
            nextP[4][23] = P[4][23] + P[0][23]*SF[5] + P[1][23]*SF[3] - P[3][23]*SF[4] + P[2][23]*SPP[0] + P[13][23]*SPP[3] + P[14][23]*SPP[6] - P[15][23]*SPP[9];
            nextP[5][23] = P[5][23] + P[0][23]*SF[4] + P[2][23]*SF[3] + P[3][23]*SF[5] - P[1][23]*SPP[0] - P[13][23]*SPP[8] + P[14][23]*SPP[2] + P[15][23]*SPP[5];
            nextP[6][23] = P[6][23] + P[1][23]*SF[4] - P[2][23]*SF[5] + P[3][23]*SF[3] + P[0][23]*SPP[0] + P[13][23]*SPP[4] - P[14][23]*SPP[7] - P[15][23]*SPP[1];
            nextP[7][23] = P[7][23] + P[4][23]*dt;
            nextP[8][23] = P[8][23] + P[5][23]*dt;
            nextP[9][23] = P[9][23] + P[6][23]*dt;
            nextP[10][23] = P[10][23];
            nextP[11][23] = P[11][23];
            nextP[12][23] = P[12][23];
            nextP[13][23] = P[13][23];
            nextP[14][23] = P[14][23];
            nextP[15][23] = P[15][23];
            nextP[16][23] = P[16][23];
            nextP[17][23] = P[17][23];
            nextP[18][23] = P[18][23];
            nextP[19][23] = P[19][23];
            nextP[20][23] = P[20][23];
            nextP[21][23] = P[21][23];
            nextP[22][23] = P[22][23];
            nextP[23][23] = P[23][23];
        }
    }

    // Copy upper diagonal to lower diagonal taking advantage of symmetry
    for (uint8_t colIndex=0; colIndex<=stateIndexLim; colIndex++)
    {
        for (uint8_t rowIndex=0; rowIndex<colIndex; rowIndex++)
        {
            nextP[colIndex][rowIndex] = nextP[rowIndex][colIndex];
        }
    }

    // add the general state process noise variances
    for (uint8_t i=0; i<=stateIndexLim; i++)
    {
        nextP[i][i] = nextP[i][i] + processNoise[i];
    }

    // if the total position variance exceeds 1e4 (100m), then stop covariance
    // growth by setting the predicted to the previous values
    // This prevent an ill conditioned matrix from occurring for long periods
    // without GPS
    if ((P[7][7] + P[8][8]) > 1e4f)
    {
        for (uint8_t i=7; i<=8; i++)
        {
            for (uint8_t j=0; j<=stateIndexLim; j++)
            {
                nextP[i][j] = P[i][j];
                nextP[j][i] = P[j][i];
            }
        }
    }

    // copy covariances to output
    CopyCovariances();

    // constrain diagonals to prevent ill-conditioning
    ConstrainVariances();

    hal.util->perf_end(_perf_CovariancePrediction);
}

// zero specified range of rows in the state covariance matrix
void NavEKF3_core::zeroRows(Matrix24 &covMat, uint8_t first, uint8_t last)
{
    uint8_t row;
    for (row=first; row<=last; row++)
    {
        memset(&covMat[row][0], 0, sizeof(covMat[0][0])*24);
    }
}

// zero specified range of columns in the state covariance matrix
void NavEKF3_core::zeroCols(Matrix24 &covMat, uint8_t first, uint8_t last)
{
    uint8_t row;
    for (row=0; row<=23; row++)
    {
        memset(&covMat[row][first], 0, sizeof(covMat[0][0])*(1+last-first));
    }
}

// reset the output data to the current EKF state
void NavEKF3_core::StoreOutputReset()
{
    outputDataNew.quat = stateStruct.quat;
    outputDataNew.velocity = stateStruct.velocity;
    outputDataNew.position = stateStruct.position;
    // write current measurement to entire table
    for (uint8_t i=0; i<imu_buffer_length; i++) {
        storedOutput[i] = outputDataNew;
    }
    outputDataDelayed = outputDataNew;
    // reset the states for the complementary filter used to provide a vertical position dervative output
    posDown = stateStruct.position.z;
    posDownDerivative = stateStruct.velocity.z;
}

// Reset the stored output quaternion history to current EKF state
void NavEKF3_core::StoreQuatReset()
{
    outputDataNew.quat = stateStruct.quat;
    // write current measurement to entire table
    for (uint8_t i=0; i<imu_buffer_length; i++) {
        storedOutput[i].quat = outputDataNew.quat;
    }
    outputDataDelayed.quat = outputDataNew.quat;
}

// Rotate the stored output quaternion history through a quaternion rotation
void NavEKF3_core::StoreQuatRotate(Quaternion deltaQuat)
{
    outputDataNew.quat = outputDataNew.quat*deltaQuat;
    // write current measurement to entire table
    for (uint8_t i=0; i<imu_buffer_length; i++) {
        storedOutput[i].quat = storedOutput[i].quat*deltaQuat;
    }
    outputDataDelayed.quat = outputDataDelayed.quat*deltaQuat;
}

// calculate nav to body quaternions from body to nav rotation matrix
void NavEKF3_core::quat2Tbn(Matrix3f &Tbn, const Quaternion &quat) const
{
    // Calculate the body to nav cosine matrix
    quat.rotation_matrix(Tbn);
}

// force symmetry on the covariance matrix to prevent ill-conditioning
void NavEKF3_core::ForceSymmetry()
{
    for (uint8_t i=1; i<=stateIndexLim; i++)
    {
        for (uint8_t j=0; j<=i-1; j++)
        {
            float temp = 0.5f*(P[i][j] + P[j][i]);
            P[i][j] = temp;
            P[j][i] = temp;
        }
    }
}

// copy covariances across from covariance prediction calculation
void NavEKF3_core::CopyCovariances()
{
    // copy predicted covariances
    for (uint8_t i=0; i<=stateIndexLim; i++) {
        for (uint8_t j=0; j<=stateIndexLim; j++)
        {
            P[i][j] = nextP[i][j];
        }
    }
}

// constrain variances (diagonal terms) in the state covariance matrix to  prevent ill-conditioning
void NavEKF3_core::ConstrainVariances()
{
    for (uint8_t i=0; i<=3; i++) P[i][i] = constrain_float(P[i][i],0.0f,1.0f); // attitude error
    for (uint8_t i=4; i<=6; i++) P[i][i] = constrain_float(P[i][i],0.0f,1.0e3f); // velocities
    for (uint8_t i=7; i<=8; i++) P[i][i] = constrain_float(P[i][i],0.0f,1.0e6f);
    P[9][9] = constrain_float(P[9][9],0.0f,1.0e6f); // vertical position
    for (uint8_t i=10; i<=12; i++) P[i][i] = constrain_float(P[i][i],0.0f,sq(0.175f * dtEkfAvg)); // delta angle biases
    for (uint8_t i=13; i<=15; i++) P[i][i] = constrain_float(P[i][i],0.0f,sq(10.0f * dtEkfAvg)); // delta velocity bias
    for (uint8_t i=16; i<=18; i++) P[i][i] = constrain_float(P[i][i],0.0f,0.01f); // earth magnetic field
    for (uint8_t i=19; i<=21; i++) P[i][i] = constrain_float(P[i][i],0.0f,0.01f); // body magnetic field
    for (uint8_t i=22; i<=23; i++) P[i][i] = constrain_float(P[i][i],0.0f,1.0e3f); // wind velocity
}

// constrain states to prevent ill-conditioning
void NavEKF3_core::ConstrainStates()
{
    // quaternionsare limited between +-1
    for (uint8_t i=0; i<=3; i++) statesArray[i] = constrain_float(statesArray[i],-1.0f,1.0f);
    // velocity limit 500 m/sec (could set this based on some multiple of max airspeed * EAS2TAS)
    for (uint8_t i=4; i<=6; i++) statesArray[i] = constrain_float(statesArray[i],-5.0e2f,5.0e2f);
    // position limit 1000 km - TODO apply circular limit
    for (uint8_t i=7; i<=8; i++) statesArray[i] = constrain_float(statesArray[i],-1.0e6f,1.0e6f);
    // height limit covers home alt on everest through to home alt at SL and ballon drop
    stateStruct.position.z = constrain_float(stateStruct.position.z,-4.0e4f,1.0e4f);
    // gyro bias limit (this needs to be set based on manufacturers specs)
    for (uint8_t i=10; i<=12; i++) statesArray[i] = constrain_float(statesArray[i],-GYRO_BIAS_LIMIT*dtEkfAvg,GYRO_BIAS_LIMIT*dtEkfAvg);
    // the accelerometer bias limit is controlled by a user adjustable parameter
    for (uint8_t i=13; i<=15; i++) statesArray[i] = constrain_float(statesArray[i],-frontend->_accBiasLim*dtEkfAvg,frontend->_accBiasLim*dtEkfAvg);
    // earth magnetic field limit
    for (uint8_t i=16; i<=18; i++) statesArray[i] = constrain_float(statesArray[i],-1.0f,1.0f);
    // body magnetic field limit
    for (uint8_t i=19; i<=21; i++) statesArray[i] = constrain_float(statesArray[i],-0.5f,0.5f);
    // wind velocity limit 100 m/s (could be based on some multiple of max airspeed * EAS2TAS) - TODO apply circular limit
    for (uint8_t i=22; i<=23; i++) statesArray[i] = constrain_float(statesArray[i],-100.0f,100.0f);
    // constrain the terrain state to be below the vehicle height unless we are using terrain as the height datum
    if (!inhibitGndState) {
        terrainState = MAX(terrainState, stateStruct.position.z + rngOnGnd);
    }
}

// calculate the NED earth spin vector in rad/sec
void NavEKF3_core::calcEarthRateNED(Vector3f &omega, int32_t latitude) const
{
    float lat_rad = radians(latitude*1.0e-7f);
    omega.x  = earthRate*cosf(lat_rad);
    omega.y  = 0;
    omega.z  = -earthRate*sinf(lat_rad);
}

// initialise the earth magnetic field states using declination, suppled roll/pitch
// and magnetometer measurements and return initial attitude quaternion
Quaternion NavEKF3_core::calcQuatAndFieldStates(float roll, float pitch)
{
    // declare local variables required to calculate initial orientation and magnetic field
    float yaw;
    Matrix3f Tbn;
    Vector3f initMagNED;
    Quaternion initQuat;

    if (use_compass()) {
        // calculate rotation matrix from body to NED frame
        Tbn.from_euler(roll, pitch, 0.0f);

        // read the magnetometer data
        readMagData();

        // rotate the magnetic field into NED axes
        initMagNED = Tbn * magDataDelayed.mag;

        // calculate heading of mag field rel to body heading
        float magHeading = atan2f(initMagNED.y, initMagNED.x);

        // get the magnetic declination
        float magDecAng = use_compass() ? _ahrs->get_compass()->get_declination() : 0;

        // calculate yaw angle rel to true north
        yaw = magDecAng - magHeading;

        // calculate initial filter quaternion states using yaw from magnetometer
        // store the yaw change so that it can be retrieved externally for use by the control loops to prevent yaw disturbances following a reset
        Vector3f tempEuler;
        stateStruct.quat.to_euler(tempEuler.x, tempEuler.y, tempEuler.z);
        // this check ensures we accumulate the resets that occur within a single iteration of the EKF
        if (imuSampleTime_ms != lastYawReset_ms) {
            yawResetAngle = 0.0f;
        }
        yawResetAngle += wrap_PI(yaw - tempEuler.z);
        lastYawReset_ms = imuSampleTime_ms;
        // calculate an initial quaternion using the new yaw value
        initQuat.from_euler(roll, pitch, yaw);
        // zero the attitude covariances becasue the corelations will now be invalid
        zeroAttCovOnly();

        // calculate initial Tbn matrix and rotate Mag measurements into NED
        // to set initial NED magnetic field states
        // don't do this if the earth field has already been learned
        if (!magFieldLearned) {
            initQuat.rotation_matrix(Tbn);
            stateStruct.earth_magfield = Tbn * magDataDelayed.mag;

            // set the NE earth magnetic field states using the published declination
            // and set the corresponding variances and covariances
            alignMagStateDeclination();

            // set the remaining variances and covariances
            zeroRows(P,18,21);
            zeroCols(P,18,21);
            P[18][18] = sq(frontend->_magNoise);
            P[19][19] = P[18][18];
            P[20][20] = P[18][18];
            P[21][21] = P[18][18];

        }

        // record the fact we have initialised the magnetic field states
        recordMagReset();

        // clear mag state reset request
        magStateResetRequest = false;

    } else {
        // this function should not be called if there is no compass data but if is is, return the
        // current attitude
        initQuat = stateStruct.quat;
    }

    // return attitude quaternion
    return initQuat;
}

// zero the attitude covariances, but preserve the variances
void NavEKF3_core::zeroAttCovOnly()
{
    float varTemp[4];
    for (uint8_t index=0; index<=3; index++) {
        varTemp[index] = P[index][index];
    }
    zeroCols(P,0,3);
    zeroRows(P,0,3);
    for (uint8_t index=0; index<=3; index++) {
        P[index][index] = varTemp[index];
    }
}

// calculate the variances for the rotation vector equivalent
Vector3f NavEKF3_core::calcRotVecVariances()
{
    Vector3f rotVarVec;
    float q0 = stateStruct.quat[0];
    float q1 = stateStruct.quat[1];
    float q2 = stateStruct.quat[2];
    float q3 = stateStruct.quat[3];
    if (q0 < 0) {
        q0 = -q0;
        q1 = -q1;
        q2 = -q2;
        q3 = -q3;
    }
    float t2 = q0*q0;
    float t3 = acos(q0);
    float t4 = -t2+1.0f;
    float t5 = t2-1.0f;
    if ((t4 > 1e-9f) && (t5 < -1e-9f)) {
        float t6 = 1.0f/t5;
        float t7 = q1*t6*2.0f;
        float t8 = 1.0f/powf(t4,1.5f);
        float t9 = q0*q1*t3*t8*2.0f;
        float t10 = t7+t9;
        float t11 = 1.0f/sqrtf(t4);
        float t12 = q2*t6*2.0f;
        float t13 = q0*q2*t3*t8*2.0f;
        float t14 = t12+t13;
        float t15 = q3*t6*2.0f;
        float t16 = q0*q3*t3*t8*2.0f;
        float t17 = t15+t16;
        rotVarVec.x = t10*(P[0][0]*t10+P[1][0]*t3*t11*2.0f)+t3*t11*(P[0][1]*t10+P[1][1]*t3*t11*2.0f)*2.0f;
        rotVarVec.y = t14*(P[0][0]*t14+P[2][0]*t3*t11*2.0f)+t3*t11*(P[0][2]*t14+P[2][2]*t3*t11*2.0f)*2.0f;
        rotVarVec.z = t17*(P[0][0]*t17+P[3][0]*t3*t11*2.0f)+t3*t11*(P[0][3]*t17+P[3][3]*t3*t11*2.0f)*2.0f;
    } else {
        rotVarVec.x = 4.0f * P[1][1];
        rotVarVec.y = 4.0f * P[2][2];
        rotVarVec.z = 4.0f * P[3][3];
    }

    return rotVarVec;
}

// initialise the quaternion covariances using rotation vector variances
void NavEKF3_core::initialiseQuatCovariances(Vector3f &rotVarVec)
{
    // calculate an equivalent rotation vector from the quaternion
    float q0 = stateStruct.quat[0];
    float q1 = stateStruct.quat[1];
    float q2 = stateStruct.quat[2];
    float q3 = stateStruct.quat[3];
    if (q0 < 0) {
        q0 = -q0;
        q1 = -q1;
        q2 = -q2;
        q3 = -q3;
    }
    float delta = 2.0f*acosf(q0);
    float scaler;
    if (fabsf(delta) > 1e-6f) {
        scaler = (delta/sinf(delta*0.5f));
    } else {
        scaler = 2.0f;
    }
    float rotX = scaler*q1;
    float rotY = scaler*q2;
    float rotZ = scaler*q3;

    // autocode generated using matlab symbolic toolbox
    float t2 = rotX*rotX;
    float t4 = rotY*rotY;
    float t5 = rotZ*rotZ;
    float t6 = t2+t4+t5;
    if (t6 > 1e-9f) {
        float t7 = sqrtf(t6);
        float t8 = t7*0.5f;
        float t3 = sinf(t8);
        float t9 = t3*t3;
        float t10 = 1.0f/t6;
        float t11 = 1.0f/sqrtf(t6);
        float t12 = cosf(t8);
        float t13 = 1.0f/powf(t6,1.5f);
        float t14 = t3*t11;
        float t15 = rotX*rotY*t3*t13;
        float t16 = rotX*rotZ*t3*t13;
        float t17 = rotY*rotZ*t3*t13;
        float t18 = t2*t10*t12*0.5f;
        float t27 = t2*t3*t13;
        float t19 = t14+t18-t27;
        float t23 = rotX*rotY*t10*t12*0.5f;
        float t28 = t15-t23;
        float t20 = rotY*rotVarVec.y*t3*t11*t28*0.5f;
        float t25 = rotX*rotZ*t10*t12*0.5f;
        float t31 = t16-t25;
        float t21 = rotZ*rotVarVec.z*t3*t11*t31*0.5f;
        float t22 = t20+t21-rotX*rotVarVec.x*t3*t11*t19*0.5f;
        float t24 = t15-t23;
        float t26 = t16-t25;
        float t29 = t4*t10*t12*0.5f;
        float t34 = t3*t4*t13;
        float t30 = t14+t29-t34;
        float t32 = t5*t10*t12*0.5f;
        float t40 = t3*t5*t13;
        float t33 = t14+t32-t40;
        float t36 = rotY*rotZ*t10*t12*0.5f;
        float t39 = t17-t36;
        float t35 = rotZ*rotVarVec.z*t3*t11*t39*0.5f;
        float t37 = t15-t23;
        float t38 = t17-t36;
        float t41 = rotVarVec.x*(t15-t23)*(t16-t25);
        float t42 = t41-rotVarVec.y*t30*t39-rotVarVec.z*t33*t39;
        float t43 = t16-t25;
        float t44 = t17-t36;

        // zero all the quaternion covariances
        zeroRows(P,0,3);
        zeroCols(P,0,3);

        // Update the quaternion internal covariances using auto-code generated using matlab symbolic toolbox
        P[0][0] = rotVarVec.x*t2*t9*t10*0.25f+rotVarVec.y*t4*t9*t10*0.25f+rotVarVec.z*t5*t9*t10*0.25f;
        P[0][1] = t22;
        P[0][2] = t35+rotX*rotVarVec.x*t3*t11*(t15-rotX*rotY*t10*t12*0.5f)*0.5f-rotY*rotVarVec.y*t3*t11*t30*0.5f;
        P[0][3] = rotX*rotVarVec.x*t3*t11*(t16-rotX*rotZ*t10*t12*0.5f)*0.5f+rotY*rotVarVec.y*t3*t11*(t17-rotY*rotZ*t10*t12*0.5f)*0.5f-rotZ*rotVarVec.z*t3*t11*t33*0.5f;
        P[1][0] = t22;
        P[1][1] = rotVarVec.x*(t19*t19)+rotVarVec.y*(t24*t24)+rotVarVec.z*(t26*t26);
        P[1][2] = rotVarVec.z*(t16-t25)*(t17-rotY*rotZ*t10*t12*0.5f)-rotVarVec.x*t19*t28-rotVarVec.y*t28*t30;
        P[1][3] = rotVarVec.y*(t15-t23)*(t17-rotY*rotZ*t10*t12*0.5f)-rotVarVec.x*t19*t31-rotVarVec.z*t31*t33;
        P[2][0] = t35-rotY*rotVarVec.y*t3*t11*t30*0.5f+rotX*rotVarVec.x*t3*t11*(t15-t23)*0.5f;
        P[2][1] = rotVarVec.z*(t16-t25)*(t17-t36)-rotVarVec.x*t19*t28-rotVarVec.y*t28*t30;
        P[2][2] = rotVarVec.y*(t30*t30)+rotVarVec.x*(t37*t37)+rotVarVec.z*(t38*t38);
        P[2][3] = t42;
        P[3][0] = rotZ*rotVarVec.z*t3*t11*t33*(-0.5f)+rotX*rotVarVec.x*t3*t11*(t16-t25)*0.5f+rotY*rotVarVec.y*t3*t11*(t17-t36)*0.5f;
        P[3][1] = rotVarVec.y*(t15-t23)*(t17-t36)-rotVarVec.x*t19*t31-rotVarVec.z*t31*t33;
        P[3][2] = t42;
        P[3][3] = rotVarVec.z*(t33*t33)+rotVarVec.x*(t43*t43)+rotVarVec.y*(t44*t44);

    } else {
        // the equations are badly conditioned so use a small angle approximation
        P[0][0] = 0.0f;
        P[0][1] = 0.0f;
        P[0][2] = 0.0f;
        P[0][3] = 0.0f;
        P[1][0] = 0.0f;
        P[1][1] = 0.25f*rotVarVec.x;
        P[1][2] = 0.0f;
        P[1][3] = 0.0f;
        P[2][0] = 0.0f;
        P[2][1] = 0.0f;
        P[2][2] = 0.25f*rotVarVec.y;
        P[2][3] = 0.0f;
        P[3][0] = 0.0f;
        P[3][1] = 0.0f;
        P[3][2] = 0.0f;
        P[3][3] = 0.25f*rotVarVec.z;

    }
}

#endif // HAL_CPU_CLASS
