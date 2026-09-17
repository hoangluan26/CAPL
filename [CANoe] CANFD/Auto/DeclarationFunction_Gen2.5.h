/*@!Encoding:65001*/
/*
 * ===================================================================================
 * FILE: DeclarationFunction_OWP_Gen2.5.h
 * ===================================================================================
 *
 * DESCRIPTION:
 *   Reusable CAPL helper function library for OWP (Parking Recording) automated
 *   test cases on Gen 2.5 Built-in Camera (BLTN_CAM) system via CAN FD.
 *
 *   Based on DeclarationFunction_OWP_Gen2.h but adapted for Gen 2.5
 * differences:
 *   - CAN FD messages: HU_BLTN_CAM_02_200ms, HU_BLTN_CAM_01_00ms,
 * HU_CLOCK_01_1000ms
 *   - SMK_TrmnlCtrlGrpStaBDC: 0x1 = Power ON (instead of 0x2 = IGN in Gen 2.0)
 *   - No ACC mode in Gen 2.5 (0x1 = Power ON, not ACC)
 *   - HU_Status = 0x2 (instead of 0x1 in Gen 2.0)
 *
 * USAGE:
 *   In your .can test script, add:
 *     includes { #include "DeclarationFunction_OWP_Gen2.5.h" }
 *
 * REQUIRED VARIABLES (declare in your .can file's variables{} block):
 *   message HU_BLTN_CAM_02_200ms _HU_BLTN_CAM_02_200ms;
 *     -> CAN FD message for HU to BLTN_CAM periodic commands (UI mode,
 * settings) message HU_BLTN_CAM_01_00ms  _HU_BLTN_CAM_01_00ms;
 *     -> CAN FD message for Manual Event Recording Request (MEV_Rec_Req)
 *   message HU_CLOCK_01_1000ms   _HU_CLOCK_01_1000ms;
 *     -> CAN FD message for clock/time setting commands
 *
 * CAN SIGNAL REFERENCE:
 *   SMK_TrmnlCtrlGrpStaBDC  : Terminal control (0x0=OFF, 0x1=Power ON)
 *   BLTN_CAM_HU_UI_Mode     : UI mode response (0x0=AVN, 0x1=Built-in Cam)
 *   BLTN_CAM_RecSet_*       : Recording setting status (0x1=OFF, 0x2=ON)
 *   BLTN_CAM_RecSta_*       : Recording active status (0x0=OFF, 0x1=Recording)
 *   BLTN_CAM_SenSet_DEV     : Driving event sensitivity level (0x1~0x5 =
 * Lv.1~Lv.5) BLTN_CAM_SenSet_PEV     : Parking event sensitivity level (0x1~0x5
 * = Lv.1~Lv.5) BLTN_CAM_RanSet_PMDR     : Parking motion detection range
 * (0x1~0x5 = Lv.1~Lv.5) BLTN_CAM_Set_FR_Ver_Pos : Front camera vertical
 * position (CAN enum) BLTN_CAM_Set_RR_Ver_Pos : Rear camera vertical position
 * (CAN enum) BLTN_CAM_Set_PrkTime    : OWP parking recording time (0x1~0x14 =
 * 1Hr~20Hr) BLTN_CAM_Set_EV_BfrTime : Event before-time raw value (actual = raw
 * + 10 seconds) BLTN_CAM_Set_EV_AftTime : Event after-time raw value (actual =
 * raw + 10 seconds) BLTN_CAM_Set_Drv_TML_Int: Time-lapse interval (0x1=20min,
 * 0x2=40min, 0x5=100min) ICU_PowerAutoCutModSta  : Power auto-cut mode
 * (0x0=Factory, 0x1=Dealer, 0x2=Customer) CF_AVN_BLTN_CAM_RecReset: SD card
 * format/reset signal (0x1=Reset triggered) BLTN_CAM_SD_Format_Op_State: SD
 * format operation state (0x1=Formatting) MEV_Rec_Req             : Manual
 * Event Recording Request (0x0=Reset, 0x1=Trigger) BLTN_CAM_State          :
 * Overall camera state
 *
 * PANEL SYSTEM VARIABLES:
 *   @panel::Switch_CGWNM, @panel::Button_USMReset, @panel::Button_RecReset,
 *   @panel::Button_Timelaps_Start, @panel::Button_Timelaps_Stop,
 *   @panel::Button_SwitchEvt, @panel::LED_Ctrl_SetOWD/OWP/DEV/PEV/PMD,
 *   @sysvar::panel::TrackBar_DEV_Sen/PEV_Sen/PMD_RanSet/FR_Ver_Pos/RR_Ver_Pos,
 *   @sysvar::panel::TrackBar_OWP_TIME/EV_Time_Bfr/EV_Time_Aft/Timelaps_TIME,
 *   @sysvar::panel::EnableDrvInfo, @sysvar::IO::VN1600_1::AIN
 *
 * TABLE OF CONTENTS:
 *   Section  1: KEY / SMK CONTROL
 *   Section  2: PANEL BUTTON ACTIONS
 *   Section  3: BLTN_CAM MODE CONTROL
 *   Section  4: RECORDING TOGGLE
 *   Section  5: RECORDING STATUS CHECK
 *   Section  6: RECORDING STATUS WAIT (Polling with Timeout)
 *   Section  7: FACTORY / CUSTOMER MODE
 *   Section  8: FRS RESET DETECTION
 *   Section  9: AIN VOLTAGE
 *   Section 10: SCREENSHOT / CAPTURE
 *   Section 11: COUNTDOWN TIMER
 *   Section 12: SIGNAL LOGGING
 *   Section 13: DRIVING INFO SIMULATION
 *   Section 14: COMMON INIT SEQUENCE
 *   Section 15: BLTN_CAM SETTINGS (TrackBar Controls)
 *   Section 16: SIGNAL POLLING FUNCTIONS
 *   Section 17: CLOCK / TIME SETTING
 *   Section 18: BATTERY TYPE SELECTION
 *   Section 19: BURGLAR ALARM SIGNAL
 *   Section 20: PMD CONTROL (Parking Motion Detection)
 *   Section 21: NETWORK MANAGEMENT (CGW NM Control)
 *   Section 22: POWER SUPPLY CONTROL (TL305TP via RS232)
 *   Section 23: ENGINEER MODE (LVDS Mode Control)
 *   Section 24: TEST MONITOR PANEL CONTROL
 * ===================================================================================
 */

// =============================================================================
// FORWARD DECLARATIONS
// =============================================================================
/*
  CHECK                           SET                       CLICK              WAIT                             OTHER
  ────────────────────────────────────────────────────────────────────────────────────────────────────────────────
  Check_BLTN_CAM_UI_Mode()        KEY_ON()                 Click_USM_Reset()   Wait_For_BLTN_CAM_Mode()
  Check_RecSet_OWD_ON()           KEY_OFF()               Click_Rec_Reset      WaitFor_RecSta_OWD_Recording()
  Check_RecSet_OWD_OFF()          Set_SMK_IGN()            Click_Evt_Switch()  WaitFor_RecSta_OWP_Recording()   Send_MEV_Rec_Req()
  Check_RecSta_OWD_Recording()    Set_SMK_ACC()            Click_TML_Start()   WaitFor_RecSta_OWD_OFF()         Reset_MEV_Rec_Req()
  Check_RecSta_OWD_OFF()          Set_PowerAutoCut_Mode()  Click_TML_Stop()    WaitFor_RecSta_OWP_OFF()         Get_AIN_Voltage()
  Check_RecSet_OWP_ON()           Rec_Reset()              Enter_Engineer_Mode WaitFor_RecSta_DEV_Recording()   Get_SMK_Status_String()
  Check_RecSet_OWP_OFF()          Turn_ON_OWD()                                WaitFor_RecSta_PEV_Recording()   Enable_DrivingInfo()
  Check_RecSta_OWP_Recording()    Turn_OFF_OWD()                               WaitFor_RecSta_DEV_OFF()         Disable_DrivingInfo()
  Check_RecSta_OWP_OFF()          Turn_ON_OWP()                                WaitFor_RecSta_PEV_OFF()         Standard_Init()
  Check_RecSet_DEV_ON()           Turn_OFF_OWP()                               WaitFor_RecSta_MAR_Recording()   Set_Dealer_Mode()
  Check_RecSet_DEV_OFF()          Turn_ON_DEV()                                WaitFor_RecSta_TML_OFF()         Set_Factory_Mode()
  Check_RecSta_DEV_Recording()    Turn_OFF_DEV()                               WaitFor_RecSta_TML_Recording()   Set_Customer_Mode()
  Check_RecSta_DEV_OFF()          Turn_ON_PEV()                                Wait_For_FRS_Reset()             CaptureGraphics()
  Check_RecSet_PEV_ON()           Turn_OFF_PEV()                               WaitFor_RecSta_PMD_Recording()   Clock_Down_ms()
  Check_RecSet_PEV_OFF()          Enter_BLTN_CAM()                             WaitFor_RecSta_PMD_OFF()         Verify_PowerAutoCut_Mode()
  Check_RecSta_PEV_Recording()    Exit_BLTN_CAM()                              WaitFor_RecSta_MAR_OFF()         Monitor_RecSta_DEV_AlwaysOFF()
  Check_RecSta_PEV_OFF()          Set_DEV_Sensitivity()                        WaitForNetworkSleep              Check_Monitor_RecSta_Signal_AlwaysOFF()
  Check_RecSta_MAR_Recording()    Set_PEV_Sensitivity()                        WaitFor_PEV_Recorded_Value       Check_Monitor_RecSta_Signal_AlwaysON()
  Check_RecSta_MAR_OFF()          Set_PMD_RanSet()                             Wait_AppState_3_Or_4_Changed
  Check_RecSta_TML_Recording()    Set_EV_BeforeTime()
  Check_RecSta_TML_OFF()          Set_EV_AfterTime()
  Check_All_Recording_Signals()   Set_TML_Interval()
  Check_DEV_Sensitivity()         Set_OWP_ParkingTime()
  Check_PEV_Sensitivity()         Set_FR_Cam_VerPos()
  Check_PMD_RanSet()              Set_RR_Cam_VerPos()
  Check_EV_BeforeTime()           Set_PEV_AppNoti()
  Check_EV_AfterTime()            Set_BglrAlrm()
  Check_TML_Interval()            SET_NM_ON()
  Check_OWP_ParkingTime()         SET_NM_OFF()
  Check_OWP_ParkingTime_InRange() Turn_ON_PMD()
  Check_FR_Cam_VerPos()           Turn_OFF_PMD()
  Check_RR_Cam_VerPos()
  Check_PEV_AppNoti()
  Check_BglrAlrm()                
  Check_AIN_Below_Threshold()     
  Check_AIN_Above_Threshold()     StartTestTimer()
  Check_Set_PMD_ON()              StopTestTimer()
  Check_Set_PMD_OFF()             StartBootTimer()
  Check_Set_PMDEV_ON()            StopBootTimer()
  Check_Set_PMDEV_OFF()           SetTestStep()
  Check_Set_PMDTime_ON()          Send_Value_To_COM()
  Check_Set_PMDTime_OFF()         Set_BPlus_Off
  Check_RecSta_PMD_Recording()    Set_BPlus_On
  Check_RecSta_PMD_OFF()          Set_Attach_Font
  Check_NM()                      Set_Detach_Font
  Check_SWVerMinor3               Set_Attach_Rear
                                  Set_Detach_Rear
                                  Set_SDcard
                                  TurnOff_Output_CH2
                                  TurnOn_Output_CH2
                                  Set_relayON
                                  Set_relayOFF
*/

variables
{
  msTimer gTestMonitorTimer; // 1-second interval timer for Elapsed Time
  int gTestMonitorSeconds;   // counter for Elapsed Time

  msTimer gBootMonitorTimer; // 1-second interval timer for Boot Time
  int gBootMonitorSeconds;   // counter for Boot Time

  // Automatically merged from INF/DeclarationFunction_Gen2.5.h
  msTimer send_BDC_FD_SMK_03_200ms_timer;
  message BDC_FD_SMK_03_200ms _BDC_FD_SMK_03_200ms;
  byte KEY_STATE;
  int flag_RecSet_error = 0;
  char answer[4];

  // COM port number for RS232 (Arduino). Change here when port changes.
  int gComPort = 29;

  // OTA AppState wait context (shared helper state)
  int gOTA_AppStateTrigger = 0;
  int gOTA_AppStateWaitStart = -1;
}

// =============================================================================
// SECTION 1: KEY / SMK CONTROL
// =============================================================================
// Purpose: Control the vehicle ignition key state via CAN signal.
// Keywords: ignition, key on, key off, SMK, terminal, ACC, IGN, power state,
//           CGW NM (Gateway Network Management), vehicle wake-up, BDC
//
// Signal: SMK_TrmnlCtrlGrpStaBDC (Smart Key Terminal Control Group Status)
//   0x0 = OFF  (key removed / power off)
//   0x1 = ACC  (accessory mode - limited electronics)
//   0x2 = IGN  (ignition on - full electronics, camera system active)
//
// Typical test flow: KEY_ON() -> [run test] -> KEY_OFF()
// For full initialization with CGW NM: Set_SMK_IGN() (enables network first)
// =============================================================================

/*
 * KEY_ON()
 * --------
 * Quick ignition ON: directly sets SMK terminal to IGN (0x2).
 * Does NOT enable CGW Network Management - use Set_SMK_IGN() for full init.
 *
 * Keywords: key on, ignition on, power on, start, wake up, SMK IGN
 * Signal modified: SMK_TrmnlCtrlGrpStaBDC = 0x2
 * Return: void (fire-and-forget, no verification)
 */
void KEY_ON()
{
  SET_NM_ON();
  // testWaitForTimeout(500);
  setSignal(SMK_TrmnlCtrlGrpStaBDC, 0x2);
  Turn_ON_HU_Clock();
}

/*
 * KEY_OFF()
 * ---------
 * Quick ignition OFF: directly sets SMK terminal to OFF (0x0).
 * Simulates removing the ignition key / turning off the vehicle.
 * The camera ECU should begin parking mode operations after KEY_OFF.
 *
 * Keywords: key off, ignition off, power off, shutdown, sleep, SMK OFF
 * Signal modified: SMK_TrmnlCtrlGrpStaBDC = 0x0
 * Return: void (fire-and-forget, no verification)
 */
void KEY_OFF()
{
  SET_NM_ON();
  setSignal(SMK_TrmnlCtrlGrpStaBDC, 0x0);
}

/*
 * Set_SMK_IGN()
 * -------------
 * Full ignition ON procedure with CGW Network Management enable.
 * Steps:
 *   1. Enable CGW NM switch on panel (wakes up CAN network)
 *   2. Wait 2 seconds for network stabilization
 *   3. Set SMK terminal to IGN (0x2)
 *   4. Wait 3 seconds for ECU initialization
 *   5. Log current SMK value for confirmation
 *
 * Use this instead of KEY_ON() when starting a test from cold boot.
 *
 * Keywords: SMK IGN, full init, CGW NM, network management, ignition,
 *           panel switch, vehicle wake up, initialization
 * Panel variable: @panel::Switch_CGWNM = 1
 * Signal modified: SMK_TrmnlCtrlGrpStaBDC = 0x2
 * Return: void
 */
void Set_SMK_IGN()
{
  write("=== Setting SMK_Signal to Power ON (0x2) via panel ===");
  if ($SMK_TrmnlCtrlGrpStaBDC == 0x2)
  {
    write(">>> SET OK: SMK already Power ON (0x2), no action required <<<");
    return;
  }

  @panel::Switch_CGWNM = 1;
  testWaitForTimeout(2000);
  setSignal(SMK_TrmnlCtrlGrpStaBDC, 0x2);
  testWaitForTimeout(3000);
  write("  SMK_TrmnlCtrlGrpStaBDC = 0x%X (Power ON)", $SMK_TrmnlCtrlGrpStaBDC);
}

/*
 * Set_SMK_ACC()
 * -------------
 * Set SMK terminal to ACC (Accessory, 0x1).
 * ACC mode provides limited electronics power without full ignition.
 * Some camera features may be restricted in ACC mode.
 *
 * Keywords: SMK ACC, accessory mode, limited power, partial ignition
 * Signal modified: SMK_TrmnlCtrlGrpStaBDC = 0x1
 * Return: void
 */
void Set_SMK_ACC()
{
  write("=== Setting SMK_Signal to ACC (0x1) via panel ===");
  if ($SMK_TrmnlCtrlGrpStaBDC == 0x1)
  {
    write(">>> SET OK: SMK already ACC (0x1), no action required <<<");
    return;
  }

  setSignal(SMK_TrmnlCtrlGrpStaBDC, 0x1);
  testWaitForTimeout(2000);
  write("  SMK_TrmnlCtrlGrpStaBDC = 0x%X (ACC)", $SMK_TrmnlCtrlGrpStaBDC);
}

/*
 * Set_SMK_OFF()
 * -------------
 * Full SMK OFF procedure with CGW Network Management disable.
 * Steps:
 *   1. Set SMK terminal to OFF (0x0)
 *   2. Wait 2 seconds for state transition
 *   3. Disable CGW NM switch on panel
 *   4. Wait 2 seconds for network shutdown
 *
 * Keywords: SMK OFF, key off, terminal off, power off, CGW NM disable
 * Signal set: SMK_TrmnlCtrlGrpStaBDC = 0x0
 * Panel: Switch_CGWNM = 0
 */
void Set_SMK_OFF()
{
  write("=== Setting SMK_Signal to OFF via panel ===");
  if ($SMK_TrmnlCtrlGrpStaBDC == 0x0)
  {
    write(">>> SET OK: SMK already OFF (0x0), no action required <<<");
    return;
  }

  setSignal(SMK_TrmnlCtrlGrpStaBDC, 0x0);
  testWaitForTimeout(2000);
  //@panel::Switch_CGWNM = 0;
  // testWaitForTimeout(2000);
  write("  SMK_TrmnlCtrlGrpStaBDC = 0x%X (OFF)", $SMK_TrmnlCtrlGrpStaBDC);
}

/*
 * Get_SMK_Status_String(char result[])
 * -------------------------------------
 * Reads the current SMK terminal state and converts it to a human-readable
 * string: "IGN", "ACC", "OFF", or "Unknown".
 * Useful for logging and test report output.
 *
 * Keywords: SMK status, terminal state, read status, string conversion, debug
 * Signal read: SMK_TrmnlCtrlGrpStaBDC
 * Parameter: result[] - output char array (min 10 chars), will contain status
 * string Return: void (result written via output parameter)
 */
void Get_SMK_Status_String(char result[])
{
  byte smk_val;
  smk_val = $SMK_TrmnlCtrlGrpStaBDC;

  if (smk_val == 0x1)
    snprintf(result, 10, "%s", "Power ON");
  else if (smk_val == 0x0)
    snprintf(result, 10, "%s", "OFF");
  else
    snprintf(result, 10, "%s", "Unknown");
}

int Apply_DEV_Sensitivity_Stepwise(int target_level)
{
  int current_level;
  int next_level;
  int direction;
  int retry;
  int guard = 0;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;

  if (target_level < 1 || target_level > 5)
  {
    write(">>> ERROR: Invalid DEV sensitivity level %d (must be 1~5) <<<",
          target_level);
    return 0;
  }

  current_level = $BLTN_CAM_SenSet_DEV;
  write("  DEV Sensitivity current=0x%X, target=Lv.%d", current_level,
        target_level);

  if (current_level == target_level)
  {
    write(">>> SKIP: DEV Sensitivity already Lv.%d <<<", target_level);
    return 1;
  }

  if (current_level < 1 || current_level > 5)
  {
    write(">>> FAILED: DEV current level is out of range; cannot step safely <<<");
    return 0;
  }

  if (target_level > current_level)
    direction = 1;
  else
    direction = -1;
  next_level = current_level + direction;

  while (next_level != (target_level + direction) && guard < 10)
  {
    retry = 0;
    while ($BLTN_CAM_SenSet_DEV != next_level && retry < 5)
    {
      write("  Step DEV Sensitivity to Lv.%d (attempt %d/5)",
            next_level, retry + 1);
      @sysvar::panel::TrackBar_DEV_Sen = next_level;
      _HU_BLTN_CAM_01_00ms.BLTN_CAM_Sen_DEV_NvalueSet = next_level;
      output(_HU_BLTN_CAM_01_00ms);
      testWaitForTimeout(3000);
      retry++;
    }

    if ($BLTN_CAM_SenSet_DEV != next_level)
    {
      write(">>> FAILED: DEV Sensitivity did not reach Lv.%d. Actual=0x%X <<<",
            next_level, $BLTN_CAM_SenSet_DEV);
      return 0;
    }

    next_level += direction;
    guard++;
  }

  return Check_DEV_Sensitivity(target_level);
}

int Apply_PEV_Sensitivity_Stepwise(int target_level)
{
  int current_level;
  int next_level;
  int direction;
  int retry;
  int guard = 0;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;

  if (target_level < 1 || target_level > 5)
  {
    write(">>> ERROR: Invalid PEV sensitivity level %d (must be 1~5) <<<",
          target_level);
    return 0;
  }

  current_level = $BLTN_CAM_SenSet_PEV;
  write("  PEV Sensitivity current=0x%X, target=Lv.%d", current_level,
        target_level);

  if (current_level == target_level)
  {
    write(">>> SKIP: PEV Sensitivity already Lv.%d <<<", target_level);
    return 1;
  }

  if (current_level < 1 || current_level > 5)
  {
    write(">>> FAILED: PEV current level is out of range; cannot step safely <<<");
    return 0;
  }

  if (target_level > current_level)
    direction = 1;
  else
    direction = -1;
  next_level = current_level + direction;

  while (next_level != (target_level + direction) && guard < 10)
  {
    retry = 0;
    while ($BLTN_CAM_SenSet_PEV != next_level && retry < 5)
    {
      write("  Step PEV Sensitivity to Lv.%d (attempt %d/5)",
            next_level, retry + 1);
      @sysvar::panel::TrackBar_PEV_Sen = next_level;
      _HU_BLTN_CAM_01_00ms.BLTN_CAM_Sen_PEV_NvalueSet = next_level;
      output(_HU_BLTN_CAM_01_00ms);
      testWaitForTimeout(3000);
      retry++;
    }

    if ($BLTN_CAM_SenSet_PEV != next_level)
    {
      write(">>> FAILED: PEV Sensitivity did not reach Lv.%d. Actual=0x%X <<<",
            next_level, $BLTN_CAM_SenSet_PEV);
      return 0;
    }

    next_level += direction;
    guard++;
  }

  return Check_PEV_Sensitivity(target_level);
}

int EV_Before_Trackbar_To_Nvalue(int trackbar_val)
{
  if (trackbar_val == 1)
    return 0xA;
  if (trackbar_val == 2)
    return 0x5;
  return 0x0;
}

int EV_Before_Current_Trackbar()
{
  int raw_value;
  raw_value = $BLTN_CAM_Set_EV_BfrTime;
  if (raw_value == 0xA)
    return 1;
  if (raw_value == 0x5)
    return 2;
  if (raw_value == 0x0)
    return 3;
  return 0;
}

int Apply_EV_BeforeTime_Stepwise(int target_trackbar)
{
  int current_trackbar;
  int next_trackbar;
  int direction;
  int retry;
  int guard = 0;
  int nvalue;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;

  if (target_trackbar < 1 || target_trackbar > 3)
  {
    write(">>> ERROR: Invalid EV Before trackbar %d (must be 1~3) <<<",
          target_trackbar);
    return 0;
  }

  current_trackbar = EV_Before_Current_Trackbar();
  write("  EV Before current trackbar=%d, target trackbar=%d",
        current_trackbar, target_trackbar);

  if (current_trackbar == target_trackbar)
  {
    write(">>> SKIP: EV Before Time already at requested trackbar %d <<<",
          target_trackbar);
    return 1;
  }

  if (current_trackbar == 0)
  {
    write(">>> FAILED: EV Before current value is out of range; cannot step safely <<<");
    return 0;
  }

  if (target_trackbar > current_trackbar)
    direction = 1;
  else
    direction = -1;
  next_trackbar = current_trackbar + direction;

  while (next_trackbar != (target_trackbar + direction) && guard < 10)
  {
    nvalue = EV_Before_Trackbar_To_Nvalue(next_trackbar);
    retry = 0;
    while (Check_EV_BeforeTime(next_trackbar) == 0 && retry < 5)
    {
      write("  Step EV Before Time to trackbar %d (NvalueSet=0x%X, attempt %d/5)",
            next_trackbar, nvalue, retry + 1);
      @sysvar::panel::TrackBar_EV_Time_Bfr = next_trackbar;
      _HU_BLTN_CAM_01_00ms.BLTN_CAM_EV_BfrTime_NvalueSet = nvalue;
      output(_HU_BLTN_CAM_01_00ms);
      testWaitForTimeout(3000);
      retry++;
    }

    if (Check_EV_BeforeTime(next_trackbar) == 0)
    {
      write(">>> FAILED: EV Before Time did not reach trackbar %d <<<",
            next_trackbar);
      return 0;
    }

    next_trackbar += direction;
    guard++;
  }

  return Check_EV_BeforeTime(target_trackbar);
}

int EV_After_Current_Seconds()
{
  int raw_value;
  raw_value = $BLTN_CAM_Set_EV_AftTime;
  if (raw_value == 0x0)
    return 10;
  if (raw_value == 0x5)
    return 15;
  if (raw_value == 0xA)
    return 20;
  return 0;
}

int Apply_EV_AfterTime_Stepwise(int target_seconds)
{
  int current_seconds;
  int next_seconds;
  int direction;
  int retry;
  int guard = 0;
  int nvalue;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;

  if (target_seconds != 10 && target_seconds != 15 && target_seconds != 20)
  {
    write(">>> ERROR: Invalid EV After seconds %d (must be 10, 15, or 20) <<<",
          target_seconds);
    return 0;
  }

  current_seconds = EV_After_Current_Seconds();
  write("  EV After current=%ds, target=%ds", current_seconds,
        target_seconds);

  if (current_seconds == target_seconds)
  {
    write(">>> SKIP: EV After Time already %ds <<<", target_seconds);
    return 1;
  }

  if (current_seconds == 0)
  {
    write(">>> FAILED: EV After current value is out of range; cannot step safely <<<");
    return 0;
  }

  if (target_seconds > current_seconds)
    direction = 5;
  else
    direction = -5;
  next_seconds = current_seconds + direction;

  while (next_seconds != (target_seconds + direction) && guard < 10)
  {
    nvalue = next_seconds - 10;
    retry = 0;
    while (Check_EV_AfterTime(next_seconds) == 0 && retry < 5)
    {
      write("  Step EV After Time to %ds (NvalueSet=0x%X, attempt %d/5)",
            next_seconds, nvalue, retry + 1);
      @sysvar::panel::TrackBar_EV_Time_Aft = next_seconds;
      _HU_BLTN_CAM_01_00ms.BLTN_CAM_EV_AftTime_NvalueSet = nvalue;
      output(_HU_BLTN_CAM_01_00ms);
      testWaitForTimeout(3000);
      retry++;
    }

    if (Check_EV_AfterTime(next_seconds) == 0)
    {
      write(">>> FAILED: EV After Time did not reach %ds <<<", next_seconds);
      return 0;
    }

    next_seconds += direction;
    guard++;
  }

  return Check_EV_AfterTime(target_seconds);
}

// =============================================================================
// SECTION 2: PANEL BUTTON ACTIONS
// =============================================================================
//
// Purpose: Simulate user button clicks on the CANoe panel GUI.
//          These correspond to physical button interactions on the camera
//          settings interface.
// Keywords: panel, button, click, GUI interaction, USM reset, Rec reset,
//           SD card format, time-lapse start/stop, event switch, MAR trigger,
//           manual event recording, MEV request, CAN message send
//
// Panel buttons use toggle behavior: set to 0 then 1 to simulate a click.
// Some buttons only need setting to 1 (latching behavior).
// =============================================================================

/*
 * Click_USM_Reset()
 * -----------------
 * Clicks the USM (Update Storage Memory) Reset button on the panel.
 * This resets the USB storage/memory module, useful before starting tests
 * to ensure clean storage state.
 *
 * Keywords: USM reset, storage reset, memory reset, panel button,
 * initialization Panel variable: @panel::Button_USMReset = 1 Wait: 5000ms after
 * click for reset to complete Return: void
 */
void Click_USM_Reset()
{
  int retry;
  int is_on;
  int dev_sensitivity_level = 5;
  int pev_sensitivity_level = 1;
  message HU_BLTN_CAM_02_200ms _HU_BLTN_CAM_02_200ms;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;
  Turn_ON_HU_Clock();
  Set_Attach_Rear();
  Set_Attach_Font();
  write("=== Clicking USM Reset button on panel ===");
  testWaitForTimeout(3000);
  @panel::Button_USMReset = 1;
  testWaitForTimeout(1000);
  write("  USM Reset clicked");

  write("=== Enter BLTN_CAM mode for USM reset post-setting ===");
  retry = 0;
  while (getSignal(BLTN_CAM_HU_UI_Mode) != 0x1 && retry < 20)
  {
    write("  Attempt %d: Entering BLTN_CAM mode...", retry + 1);
    _HU_BLTN_CAM_02_200ms.HU_BLTN_CAM_UI_Mode = 0x0;
    output(_HU_BLTN_CAM_02_200ms);
    testWaitForTimeout(1000);
    _HU_BLTN_CAM_02_200ms.HU_BLTN_CAM_UI_Mode = 0x1;
    output(_HU_BLTN_CAM_02_200ms);
    testWaitForTimeout(3000);
    retry++;
  }

  if (getSignal(BLTN_CAM_HU_UI_Mode) == 0x1)
  {
    write(">>> SUCCESS: BLTN_CAM mode confirmed <<<");
  }
  else
  {
    write(">>> FAILED: Could not enter BLTN_CAM mode after 10 attempts. BLTN_CAM_HU_UI_Mode = 0x%X <<<",
          getSignal(BLTN_CAM_HU_UI_Mode));
    return;
  }

  testWaitForTimeout(3000);

  write("=== Turn ON OWD via panel without leaving BLTN_CAM ===");
  retry = 0;
  is_on = Check_RecSet_OWD_ON();
  while (is_on == 0 && retry < 10)
  {
    write("  Attempt %d: Force OWD OFF/default edge, then OWD ON...", retry + 1);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_OWD_NvalueSet = 0x1;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_OWD_NvalueSet = 0x1");
    testWaitForTimeout(500);

    @panel::LED_Ctrl_SetOWD = 0;
    testWaitForTimeout(100);
    @panel::LED_Ctrl_SetOWD = 1;
    testWaitForTimeout(200);
    @panel::LED_Ctrl_SetOWD = 0;
    testWaitForTimeout(500);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_OWD_NvalueSet = 0x2;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_OWD_NvalueSet = 0x2");

    testWaitForTimeout(3000);
    is_on = Check_RecSet_OWD_ON();
    retry++;
  }

  if (is_on == 1)
    write(">>> SUCCESS: OWD = ON (0x2) <<<");
  else
    write(">>> FAILED: Could not set OWD to ON after 10 attempts <<<");

  write("=== Turn ON OWP via panel without leaving BLTN_CAM ===");
  retry = 0;
  is_on = Check_RecSet_OWP_ON();
  while (is_on == 0 && retry < 10)
  {
    write("  Attempt %d: Force OWP OFF/default edge, then OWP ON...", retry + 1);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_OWP_NvalueSet = 0x1;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_OWP_NvalueSet = 0x1");
    testWaitForTimeout(500);

    @panel::LED_Ctrl_SetOWP = 0;
    testWaitForTimeout(100);
    @panel::LED_Ctrl_SetOWP = 1;
    testWaitForTimeout(200);
    @panel::LED_Ctrl_SetOWP = 0;
    testWaitForTimeout(500);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_OWP_NvalueSet = 0x2;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_OWP_NvalueSet = 0x2");

    testWaitForTimeout(3000);
    is_on = Check_RecSet_OWP_ON();
    retry++;
  }

  if (is_on == 1)
    write(">>> SUCCESS: OWP = ON (0x2) <<<");
  else
    write(">>> FAILED: Could not set OWP to ON after 10 attempts <<<");

  write("=== Turn ON PEV via panel without leaving BLTN_CAM ===");
  retry = 0;
  is_on = Check_RecSet_PEV_ON();
  while (is_on == 0 && retry < 10)
  {
    write("  Attempt %d: Force PEV OFF/default edge, then PEV ON...", retry + 1);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_PEV_NvalueSet = 0x1;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_PEV_NvalueSet = 0x1");
    testWaitForTimeout(500);

    @panel::LED_Ctrl_SetPEV = 0;
    testWaitForTimeout(100);
    @panel::LED_Ctrl_SetPEV = 1;
    testWaitForTimeout(200);
    @panel::LED_Ctrl_SetPEV = 0;
    testWaitForTimeout(500);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_PEV_NvalueSet = 0x2;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_PEV_NvalueSet = 0x2");

    testWaitForTimeout(3000);
    is_on = Check_RecSet_PEV_ON();
    retry++;
  }

  if (is_on == 1)
    write(">>> SUCCESS: PEV = ON (0x2) <<<");
  else
    write(">>> FAILED: Could not set PEV to ON after 10 attempts <<<");

  write("=== Turn ON DEV via panel without leaving BLTN_CAM ===");
  retry = 0;
  is_on = Check_RecSet_DEV_ON();
  while (is_on == 0 && retry < 10)
  {
    write("  Attempt %d: Force DEV OFF/default edge, then DEV ON...", retry + 1);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_DEV_NvalueSet = 0x1;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_DEV_NvalueSet = 0x1");
    testWaitForTimeout(500);

    @panel::LED_Ctrl_SetDEV = 0;
    testWaitForTimeout(100);
    @panel::LED_Ctrl_SetDEV = 1;
    testWaitForTimeout(200);
    @panel::LED_Ctrl_SetDEV = 0;
    testWaitForTimeout(500);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_DEV_NvalueSet = 0x2;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_DEV_NvalueSet = 0x2");

    testWaitForTimeout(3000);
    is_on = Check_RecSet_DEV_ON();
    retry++;
  }

  if (is_on == 1)
    write(">>> SUCCESS: DEV = ON (0x2) <<<");
  else
    write(">>> FAILED: Could not set DEV to ON after 10 attempts <<<");

  write("=== Set DEV Sensitivity to Lv.%d without leaving BLTN_CAM ===",
        dev_sensitivity_level);
  // DEV sensitivity manual edit mapping:
  //   1 / 0x1 = Level 1
  //   2 / 0x2 = Level 2
  //   3 / 0x3 = Level 3
  //   4 / 0x4 = Level 4
  //   5 / 0x5 = Level 5
  // Manual edit point: change dev_sensitivity_level above to 1~5.
  // Use the same value for TrackBar_DEV_Sen and BLTN_CAM_Sen_DEV_NvalueSet.
  if (Apply_DEV_Sensitivity_Stepwise(dev_sensitivity_level) == 1)
    write(">>> SUCCESS: DEV Sensitivity = Lv.%d <<<", dev_sensitivity_level);
  else
    write(">>> FAILED: Could not set DEV Sensitivity to Lv.%d. BLTN_CAM_SenSet_DEV = 0x%X <<<",
          dev_sensitivity_level, $BLTN_CAM_SenSet_DEV);

  write("=== Set PEV Sensitivity to Lv.%d without leaving BLTN_CAM ===",
        pev_sensitivity_level);
  // PEV sensitivity manual edit mapping:
  //   1 / 0x1 = Level 1
  //   2 / 0x2 = Level 2
  //   3 / 0x3 = Level 3
  //   4 / 0x4 = Level 4
  //   5 / 0x5 = Level 5
  // Manual edit point: change pev_sensitivity_level above to 1~5.
  // Use the same value for TrackBar_PEV_Sen and BLTN_CAM_Sen_PEV_NvalueSet.
  if (Apply_PEV_Sensitivity_Stepwise(pev_sensitivity_level) == 1)
    write(">>> SUCCESS: PEV Sensitivity = Lv.%d <<<", pev_sensitivity_level);
  else
    write(">>> FAILED: Could not set PEV Sensitivity to Lv.%d. BLTN_CAM_SenSet_PEV = 0x%X <<<",
          pev_sensitivity_level, $BLTN_CAM_SenSet_PEV);

  write("=== Ensure EV Before Time = 10s without leaving BLTN_CAM ===");
  if (Apply_EV_BeforeTime_Stepwise(3) == 1)
    write(">>> SUCCESS: EV Before Time = 10s <<<");
  else
    write(">>> FAILED: Could not set EV Before Time to 10s. BLTN_CAM_Set_EV_BfrTime = 0x%X <<<",
          $BLTN_CAM_Set_EV_BfrTime);

  write("=== Ensure EV After Time = 10s without leaving BLTN_CAM ===");
  if (Apply_EV_AfterTime_Stepwise(10) == 1)
    write(">>> SUCCESS: EV After Time = 10s <<<");
  else
    write(">>> FAILED: Could not set EV After Time to 10s. BLTN_CAM_Set_EV_AftTime = 0x%X <<<",
          $BLTN_CAM_Set_EV_AftTime);

  write("=== Exit BLTN_CAM mode after USM reset post-setting ===");
  retry = 0;
  while (getSignal(BLTN_CAM_HU_UI_Mode) != 0x0 && retry < 20)
  {
    write("  Attempt %d: Exiting to AVN mode...", retry + 1);
    _HU_BLTN_CAM_02_200ms.HU_BLTN_CAM_UI_Mode = 0x0;
    output(_HU_BLTN_CAM_02_200ms);
    testWaitForTimeout(1000);
    retry++;
  }

  if (getSignal(BLTN_CAM_HU_UI_Mode) == 0x0)
    write(">>> SUCCESS: AVN mode confirmed <<<");
  else
    write(">>> FAILED: Could not exit BLTN_CAM mode after 10 attempts. BLTN_CAM_HU_UI_Mode = 0x%X <<<",
          getSignal(BLTN_CAM_HU_UI_Mode));
}

/*
 * Click_Rec_Reset()
 * -----------------
 * Clicks the Recording Reset button on the panel.
 * Initiates SD card format / recording data reset.
 * Used to clear all recorded files and start fresh.
 *
 * Keywords: recording reset, SD card format, clear recordings, panel button,
 *           data reset, storage cleanup, initialization
 * Panel variable: @panel::Button_RecReset = 1
 * Wait: 2000ms after click
 * Return: void
 */
void Click_Rec_Reset()
{
  /*
  write("=== Clicking Rec Reset button on panel ===");
  @panel::Button_RecReset = 1;
  testWaitForTimeout(2000);
  write("  Rec Reset clicked");
  */
}

/*
 * Click_TML_Start()
 * -----------------
 * Clicks the Time-lapse Start button on the panel.
 * Initiates time-lapse (TML) recording mode.
 * Uses toggle pattern: set to 0 first, then 1 (simulates button press).
 *
 * Keywords: time-lapse, TML, start recording, timelapse start, panel button
 * Panel variable: @panel::Button_Timelaps_Start (0 -> 1 toggle)
 * Wait: 500ms between toggle + 3000ms after click
 * Return: void
 */
void Click_TML_Start()
{
  Click_TML_Stop(); // Ensure TML is stopped before starting
  testWaitForTimeout(3000);

  write("=== Clicking TML Start button on panel ===");
  @panel::Button_Timelaps_Start = 0;
  testWaitForTimeout(500);
  @panel::Button_Timelaps_Start = 1;
  testWaitForTimeout(3000);
  write("  TML Start clicked");
}

/*
 * Click_TML_Stop()
 * ----------------
 * Clicks the Time-lapse Stop button on the panel.
 * Stops ongoing time-lapse (TML) recording.
 * Uses toggle pattern: set to 0 first, then 1 (simulates button press).
 *
 * Keywords: time-lapse, TML, stop recording, timelapse stop, panel button
 * Panel variable: @panel::Button_Timelaps_Stop (0 -> 1 toggle)
 * Wait: 500ms between toggle + 3000ms after click
 * Return: void
 */
void Click_TML_Stop()
{
  write("=== Clicking TML Stop button on panel ===");
  @panel::Button_Timelaps_Stop = 0;
  testWaitForTimeout(500);
  @panel::Button_Timelaps_Stop = 1;
  testWaitForTimeout(3000);
  write("  TML Stop clicked");
}

/*
 * Click_Evt_Switch()
 * ------------------
 * Clicks the Event Switch button on the panel.
 * Triggers a manual recording event (MAR - Manual Recording).
 * In real vehicle, this is equivalent to pressing the physical event button
 * on the dashcam to save the current moment.
 * Uses toggle pattern: set to 0 first, then 1 (simulates button press).
 *
 * Keywords: event switch, MAR, manual recording, manual event, trigger event,
 *           save moment, panel button, dashcam event button
 * Panel variable: @panel::Button_SwitchEvt (0 -> 1 toggle)
 * Pre-reset: MEV_Rec_Req = 0x0 via CAN (200ms) to ensure ECU accepts next
 * trigger Wait: 500ms between toggle + 3000ms after click Return: void
 */
void Click_Evt_Switch()
{
  int i;
  byte mar_status;

  // Reset MEV signal trước
  message HU_BLTN_CAM_01_00ms _reset;
  _reset.MEV_Rec_Req = 0x0;
  output(_reset);
  testWaitForTimeout(200);

  write("=== Clicking Event Switch button on panel ===");
  @panel::Button_SwitchEvt = 0;
  testWaitForTimeout(500);
  @panel::Button_SwitchEvt = 1;
  testWaitForTimeout(1000);
  write("  Event Switch clicked");

  // Check MAR (Manual Recording) signal and retry if not set
  write("  Checking MAR (Manual Recording) signal...");
  for (i = 0; i < 15; i++)
  {
    mar_status = $BLTN_CAM_RecSta_MAR;
    write("    Attempt %d: BLTN_CAM_RecSta_MAR = 0x%X", i + 1, mar_status);

    if (mar_status == 0x1)
    {
      write("  [OK] MAR Recording confirmed");
      return;
    }

    testWaitForTimeout(500);
    @panel::Button_SwitchEvt = 0;
    testWaitForTimeout(300);
    @panel::Button_SwitchEvt = 1;
    testWaitForTimeout(1000);
  }

  write("  [NG] MAR Recording not confirmed after 15 attempts");
}

/*
 * Send_MEV_Rec_Req()
 * ------------------
 * Sends a Manual Event Recording Request via CAN message.
 * This is the CAN-level method to trigger a manual recording event,
 * as opposed to panel button click.
 *
 * Process:
 *   1. Fill all 8 bytes of HU_BLTN_CAM_01_00ms with 0xFF (default padding)
 *   2. Set MEV_Rec_Req signal to 0x1 (Trigger)
 *   3. Send the CAN message via output()
 *
 * Keywords: MEV, manual event recording, CAN message, trigger,
 * HU_BLTN_CAM_01_00ms, MEV_Rec_Req, recording request, event trigger via CAN
 * CAN message: HU_BLTN_CAM_01_00ms
 * Signal: MEV_Rec_Req = 0x1 (Trigger)
 * Wait: 3000ms after sending
 * Return: void
 */
void Send_MEV_Rec_Req()
{
  int i;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;
  write("=== Sending MEV_Rec_Req = 0x1 via CAN ===");
  for (i = 0; i < 8; i++)
  {
    _HU_BLTN_CAM_01_00ms.byte(i) = 0xFF;
  }
  _HU_BLTN_CAM_01_00ms.MEV_Rec_Req = 0x1;
  output(_HU_BLTN_CAM_01_00ms);
  testWaitForTimeout(3000);
  write("  MEV_Rec_Req sent");
}

/*
 * Reset_MEV_Rec_Req()
 * --------------------
 * Resets the Manual Event Recording Request signal back to 0x0.
 * Must be called after Send_MEV_Rec_Req() to clear the request,
 * otherwise the signal stays asserted.
 *
 * Keywords: MEV reset, clear event request, MEV_Rec_Req clear, CAN message
 * reset CAN message: HU_BLTN_CAM_01_00ms Signal: MEV_Rec_Req = 0x0 (Reset)
 * Wait: 1000ms after sending
 * Return: void
 */
void Reset_MEV_Rec_Req()
{
  int i;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;
  write("=== Resetting MEV_Rec_Req = 0x0 ===");
  for (i = 0; i < 8; i++)
  {
    _HU_BLTN_CAM_01_00ms.byte(i) = 0xFF;
  }
  _HU_BLTN_CAM_01_00ms.MEV_Rec_Req = 0x0;
  output(_HU_BLTN_CAM_01_00ms);
  testWaitForTimeout(1000);
}

// =============================================================================
// SECTION 3: BLTN_CAM MODE CONTROL (Enter / Exit Built-in Camera)
// =============================================================================
//
// Purpose: Switch the Head Unit (HU) display between AVN mode and Built-in
//          Camera mode. The camera settings UI is only accessible when in
//          Built-in Camera mode.
// Keywords: BLTN_CAM, built-in camera, UI mode, AVN mode, head unit display,
//           HU_BLTN_CAM_UI_Mode, enter camera, exit camera, mode switch,
//           HU_BLTN_CAM_02_200ms, BLTN_CAM_HU_UI_Mode
//
// UI Mode values:
//   HU_BLTN_CAM_UI_Mode = 0x0 : AVN mode (normal Audio/Video/Navigation)
//   HU_BLTN_CAM_UI_Mode = 0x1 : Built-in Camera mode (dashcam settings visible)
//
// Response signal: BLTN_CAM_HU_UI_Mode (feedback from camera ECU)
//   0x0 = AVN mode confirmed
//   0x1 = Built-in Camera mode confirmed
// =============================================================================

/*
 * Enter_BLTN_CAM()
 * ----------------
 * Switches HU display to Built-in Camera mode by sending UI_Mode=0x1
 * via CAN message HU_BLTN_CAM_02_200ms.
 * Retries sending UI_Mode=0x1 up to 15 times (500ms interval = 7.5s total),
 * re-sending the CAN message on each attempt until the mode is confirmed.
 *
 * Keywords: enter camera mode, open camera UI, BLTN_CAM mode on,
 *           HU_BLTN_CAM_02_200ms, switch to camera
 * CAN message: HU_BLTN_CAM_02_200ms
 * Signal sent: HU_BLTN_CAM_UI_Mode = 0x1
 * Signal checked: BLTN_CAM_HU_UI_Mode == 0x1
 * Return: void (logs OK/NG)
 */
void Enter_BLTN_CAM()
{

  int i;
  message HU_BLTN_CAM_02_200ms _HU_BLTN_CAM_02_200ms;
  write(">>> Enter BLTN_CAM mode <<<");
  KEY_ON();
  for (i = 0; i < 20; i++)
  {
    _HU_BLTN_CAM_02_200ms.HU_BLTN_CAM_UI_Mode = 0x0;
    output(_HU_BLTN_CAM_02_200ms);
    testWaitForTimeout(2000);
    _HU_BLTN_CAM_02_200ms.HU_BLTN_CAM_UI_Mode = 0x1;
    output(_HU_BLTN_CAM_02_200ms);
    testWaitForTimeout(3000);
    if (getSignal(BLTN_CAM_HU_UI_Mode) == 0x1)
    {
      write(">>> [OK] BLTN_CAM mode confirmed <<<");
      return;
    }
  }
  write(">>> [NG] Timeout: BLTN_CAM_HU_UI_Mode = 0x%X <<<",
        getSignal(BLTN_CAM_HU_UI_Mode));
}

/*
 * Exit_BLTN_CAM()
 * ---------------
 * Switches HU display back to AVN (Audio/Video/Navigation) mode
 * by sending UI_Mode=0x0 via CAN message.
 * Retries sending UI_Mode=0x0 up to 15 times (500ms interval = 7.5s total),
 * re-sending the CAN message on each attempt until the mode is confirmed.
 *
 * Keywords: exit camera mode, close camera UI, back to AVN, BLTN_CAM mode off,
 *           return to navigation, switch to AVN
 * CAN message: HU_BLTN_CAM_02_200ms
 * Signal sent: HU_BLTN_CAM_UI_Mode = 0x0
 * Signal checked: BLTN_CAM_HU_UI_Mode == 0x0
 * Return: void (logs OK/NG)
 */
void Exit_BLTN_CAM()
{

  int i;
  message HU_BLTN_CAM_02_200ms _HU_BLTN_CAM_02_200ms;
  write(">>> Exit to AVN mode <<<");
  KEY_ON();
  for (i = 0; i < 15; i++)
  {
    _HU_BLTN_CAM_02_200ms.HU_BLTN_CAM_UI_Mode = 0x0;
    output(_HU_BLTN_CAM_02_200ms);
    testWaitForTimeout(1000);
    if (getSignal(BLTN_CAM_HU_UI_Mode) == 0x0)
    {
      write(">>> [OK] AVN mode confirmed <<<");
      return;
    }
  }
  write(">>> [NG] Timeout: BLTN_CAM_HU_UI_Mode = 0x%X <<<",
        getSignal(BLTN_CAM_HU_UI_Mode));
}

/*
 * Wait_For_BLTN_CAM_Mode(long timeout_ms)
 * ----------------------------------------
 * Waits for the Built-in Camera mode to become active, with a configurable
 * timeout. Used when the mode change is triggered externally (e.g., by
 * the user or another test step) and we just need to wait for it.
 * Periodically logs the current state at ~1/6 of total timeout intervals.
 *
 * Keywords: wait for camera mode, polling, timeout, BLTN_CAM_HU_UI_Mode,
 *           mode detection, manual entry, wait for UI
 * Signal checked: BLTN_CAM_HU_UI_Mode == 0x1
 * Parameter: timeout_ms - maximum wait time in milliseconds
 * Return: 1 = camera mode detected, 0 = timeout
 */
int Wait_For_BLTN_CAM_Mode(long timeout_ms)
{
  long elapsed = 0;
  long log_interval;
  byte ui_mode = 0;

  write("Waiting for Built-in Cam mode... (max %d seconds)", timeout_ms / 1000);

  log_interval = (timeout_ms / 6 / 1000) * 1000;
  if (log_interval < 1000)
    log_interval = 1000;

  while (elapsed < timeout_ms)
  {
    ui_mode = $BLTN_CAM_HU_UI_Mode;

    if (ui_mode == 0x1)
    {
      write(">>> Now in Built-in Cam mode! <<<");
      return 1;
    }

    if (elapsed == 0 || elapsed % log_interval == 0)
    {
      write("  [%d s] UI_Mode=0x%X (0=AVN, 1=BLTN_CAM)", elapsed / 1000,
            ui_mode);
    }

    testWaitForTimeout(1000);
    elapsed += 1000;
  }

  write(">>> TIMEOUT: Not in Built-in Cam mode <<<");
  return 0;
}

/*
 * Check_BLTN_CAM_UI_Mode(byte expected_mode)
 * -------------------------------------------
 * Instantly checks whether the current BLTN_CAM_HU_UI_Mode matches the
 * expected value. Waits 2 seconds before reading for signal stabilization.
 *
 * Keywords: check UI mode, verify mode, BLTN_CAM_HU_UI_Mode, instant check,
 *           AVN mode check, camera mode check, mode verification
 * Signal checked: BLTN_CAM_HU_UI_Mode
 * Parameter: expected_mode - 0x0 (AVN) or 0x1 (Built-in Cam)
 * Return: 1 = PASS (mode matches), 0 = FAIL (mode mismatch)
 */
int Check_BLTN_CAM_UI_Mode(byte expected_mode)
{
  byte ui_mode = 0;

  testWaitForTimeout(2000);

  ui_mode = $BLTN_CAM_HU_UI_Mode;
  write("  BLTN_CAM_HU_UI_Mode = 0x%X", ui_mode);

  if (ui_mode == expected_mode)
  {
    if (expected_mode == 0x0)
      write(">>> PASS: BLTN_CAM_HU_UI_Mode = 0x0 (AVN Mode) <<<");
    else if (expected_mode == 0x1)
      write(">>> PASS: BLTN_CAM_HU_UI_Mode = 0x1 (Build-in Cam Mode) <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: BLTN_CAM_HU_UI_Mode != 0x%X <<<", expected_mode);
    return 0;
  }
}

// =============================================================================
// SECTION 4: RECORDING TOGGLE (Panel LED Controls)
// =============================================================================
//
// Purpose: Turn ON or OFF each recording type through the CANoe panel LED
//          toggle controls. Each function includes a retry loop to handle
//          cases where the ECU doesn't respond immediately.
// Keywords: recording toggle, turn on, turn off, LED control, panel,
//           OWD, OWP, DEV, PEV, PMD, continuous recording, event recording,
//           parking recording, driving recording, motion detection,
//           RecSet signal, retry loop, toggle button
//
// Recording types:
//   OWD = Continuous Driving Recording (records while driving)
//   OWP = Continuous Parking Recording (records while parked)
//   DEV = Driving Event Recording (saves clip on driving impact/event)
//   PEV = Parking Event Recording (saves clip on parking impact/event)
//   PMD = Parking Motion Detection (records on motion detected while parked)
//
// RecSet signal values (setting status, response from camera ECU):
//   0x1 = OFF (recording function disabled)
//   0x2 = ON  (recording function enabled)
//
// Note: These are toggle controls - clicking when ON turns OFF, and vice versa.
//       The retry loop checks the signal after each click attempt.
//       Turn_ON retries up to 10 times, Turn_OFF retries up to 15 times.
// =============================================================================

/*
 * Turn_ON_OWD()
 * -------------
 * Enables Continuous Driving Recording (OWD) via panel LED toggle.
 * Retries up to 10 times until BLTN_CAM_RecSet_OWD == 0x2 (ON).
 *
 * Keywords: OWD on, enable driving recording, continuous driving, LED toggle,
 *           BLTN_CAM_RecSet_OWD, panel LED_Ctrl_SetOWD
 * Panel variable: @panel::LED_Ctrl_SetOWD = 1 (toggle click)
 * Signal checked: BLTN_CAM_RecSet_OWD == 0x2 (ON)
 * Retry: up to 10 attempts, 2000ms between each
 * Return: void (logs SUCCESS/FAILED)
 */
void Turn_ON_OWD()
{
  int retry = 0;
  int is_on = 0;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;

  write("=== Turn ON OWD via panel ===");

  is_on = Check_RecSet_OWD_ON();
  if (is_on == 1)
  {
    write(">>> SET OK: OWD already ON (0x2), no action required <<<");
    return;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  is_on = Check_RecSet_OWD_ON();
  while (is_on == 0 && retry < 10)
  {
    write("  Attempt %d: Force OWD OFF/default edge, then OWD ON...", retry + 1);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_OWD_NvalueSet = 0x1;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_OWD_NvalueSet = 0x1");
    testWaitForTimeout(500);

    @panel::LED_Ctrl_SetOWD = 0;
    testWaitForTimeout(100);
    @panel::LED_Ctrl_SetOWD = 1;
    testWaitForTimeout(200);
    @panel::LED_Ctrl_SetOWD = 0;
    testWaitForTimeout(500);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_OWD_NvalueSet = 0x2;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_OWD_NvalueSet = 0x2");

    testWaitForTimeout(3000);
    is_on = Check_RecSet_OWD_ON();
    retry++;
  }

  if (is_on == 1)
    write(">>> SUCCESS: OWD = ON (0x2) <<<");
  else
    write(">>> FAILED: Could not set OWD to ON <<<");

  Exit_BLTN_CAM();
  testWaitForTimeout(1000);
}

/*
 * Turn_ON_OWP()
 * -------------
 * Enables Continuous Parking Recording (OWP) via panel LED toggle.
 * Retries up to 10 times until BLTN_CAM_RecSet_OWP == 0x2 (ON).
 *
 * Keywords: OWP on, enable parking recording, continuous parking, LED toggle,
 *           BLTN_CAM_RecSet_OWP, panel LED_Ctrl_SetOWP
 * Panel variable: @panel::LED_Ctrl_SetOWP = 1
 * Signal checked: BLTN_CAM_RecSet_OWP == 0x2 (ON)
 * Retry: up to 10 attempts, 2000ms between each
 * Return: void (logs SUCCESS/FAILED)
 */
void Turn_ON_OWP()
{
  int retry = 0;
  int is_on = 0;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;

  write("=== Turn ON OWP via panel ===");

  is_on = Check_RecSet_OWP_ON();
  if (is_on == 1)
  {
    write(">>> SET OK: OWP already ON (0x2), no action required <<<");
    return;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  is_on = Check_RecSet_OWP_ON();
  while (is_on == 0 && retry < 10)
  {
    write("  Attempt %d: Force OWP OFF/default edge, then OWP ON...", retry + 1);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_OWP_NvalueSet = 0x1;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_OWP_NvalueSet = 0x1");
    testWaitForTimeout(500);

    @panel::LED_Ctrl_SetOWP = 0;
    testWaitForTimeout(100);
    @panel::LED_Ctrl_SetOWP = 1;
    testWaitForTimeout(200);
    @panel::LED_Ctrl_SetOWP = 0;
    testWaitForTimeout(500);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_OWP_NvalueSet = 0x2;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_OWP_NvalueSet = 0x2");

    testWaitForTimeout(3000);
    is_on = Check_RecSet_OWP_ON();
    retry++;
  }

  if (is_on == 1)
    write(">>> SUCCESS: OWP = ON (0x2) <<<");
  else
    write(">>> FAILED: Could not set OWP to ON <<<");

  Exit_BLTN_CAM();
  testWaitForTimeout(1000);
}

/*
 * Turn_ON_DEV()
 * -------------
 * Enables Driving Event Recording (DEV) via panel LED toggle.
 * Retries up to 10 times until BLTN_CAM_RecSet_DEV == 0x2 (ON).
 *
 * Keywords: DEV on, enable driving event, event recording, impact detection,
 *           BLTN_CAM_RecSet_DEV, panel LED_Ctrl_SetDEV
 * Panel variable: @panel::LED_Ctrl_SetDEV = 1
 * Signal checked: BLTN_CAM_RecSet_DEV == 0x2 (ON)
 * Retry: up to 10 attempts, 3000ms between each (DEV needs longer)
 * Return: void (logs SUCCESS/FAILED)
 */
void Turn_ON_DEV()
{
  int retry = 0;
  int is_on = 0;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;

  write("=== Turn ON DEV via panel ===");

  is_on = Check_RecSet_DEV_ON();
  if (is_on == 1)
  {
    write(">>> SET OK: DEV already ON (0x2), no action required <<<");
    return;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  is_on = Check_RecSet_DEV_ON();
  while (is_on == 0 && retry < 10)
  {
    write("  Attempt %d: Force DEV OFF/default edge, then DEV ON...", retry + 1);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_DEV_NvalueSet = 0x1;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_DEV_NvalueSet = 0x1");
    testWaitForTimeout(500);

    @panel::LED_Ctrl_SetDEV = 0;
    testWaitForTimeout(100);
    @panel::LED_Ctrl_SetDEV = 1;
    testWaitForTimeout(200);
    @panel::LED_Ctrl_SetDEV = 0;
    testWaitForTimeout(500);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_DEV_NvalueSet = 0x2;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_DEV_NvalueSet = 0x2");

    testWaitForTimeout(3000);
    is_on = Check_RecSet_DEV_ON();
    retry++;
  }

  if (is_on == 1)
    write(">>> SUCCESS: DEV = ON (0x2) <<<");
  else
    write(">>> FAILED: Could not set DEV to ON <<<");

  Exit_BLTN_CAM();
  testWaitForTimeout(1000);
}

/*
 * Turn_ON_PEV()
 * -------------
 * Enables Parking Event Recording (PEV) via panel LED toggle.
 * Retries up to 10 times until BLTN_CAM_RecSet_PEV == 0x2 (ON).
 *
 * Keywords: PEV on, enable parking event, parking impact, event recording,
 *           BLTN_CAM_RecSet_PEV, panel LED_Ctrl_SetPEV
 * Panel variable: @panel::LED_Ctrl_SetPEV = 1
 * Signal checked: BLTN_CAM_RecSet_PEV == 0x2 (ON)
 * Retry: up to 10 attempts, 2000ms between each
 * Return: void (logs SUCCESS/FAILED)
 */
void Turn_ON_PEV()
{
  int retry = 0;
  int is_on = 0;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;

  write("=== Turn ON PEV via panel ===");

  is_on = Check_RecSet_PEV_ON();
  if (is_on == 1)
  {
    write(">>> SET OK: PEV already ON (0x2), no action required <<<");
    return;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  is_on = Check_RecSet_PEV_ON();
  while (is_on == 0 && retry < 10)
  {
    write("  Attempt %d: Force PEV OFF/default edge, then PEV ON...", retry + 1);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_PEV_NvalueSet = 0x1;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_PEV_NvalueSet = 0x1");
    testWaitForTimeout(500);

    @panel::LED_Ctrl_SetPEV = 0;
    testWaitForTimeout(100);
    @panel::LED_Ctrl_SetPEV = 1;
    testWaitForTimeout(200);
    @panel::LED_Ctrl_SetPEV = 0;
    testWaitForTimeout(500);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_PEV_NvalueSet = 0x2;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_PEV_NvalueSet = 0x2");

    testWaitForTimeout(3000);
    is_on = Check_RecSet_PEV_ON();
    retry++;
  }

  if (is_on == 1)
    write(">>> SUCCESS: PEV = ON (0x2) <<<");
  else
    write(">>> FAILED: Could not set PEV to ON <<<");

  Exit_BLTN_CAM();
  testWaitForTimeout(1000);
}

/*
 * Apply_CCS_RecSettings(int appNotiValue, int pevSensitivity)
 * ------------------------------------------------------------
 * CCS shared setup helper: ensures OWP/PEV are ON, sets PEV app notification,
 * and optionally applies PEV sensitivity when pevSensitivCheck_Set_PMD_ONity > 0.
 *
 * Parameters:
 *   appNotiValue   : expected BLTN_CAM_Set_PEV_AppNoti value (typically 0x1/0x2)
 *   pevSensitivity : 0 to skip, or 1~5 to set BLTN_CAM_SenSet_PEV level
 */
void Apply_CCS_RecSettings(int appNotiValue, int pevSensitivity)
{
  Turn_ON_OWP();
  Turn_ON_PEV();

  if (Set_PEV_AppNoti(appNotiValue) == 0)
  {
    write("WARNING: Set_PEV_AppNoti(0x%X) failed", appNotiValue);
  }

  if (pevSensitivity > 0)
  {
    if (Set_PEV_Sensitivity(pevSensitivity) == 0)
    {
      write("WARNING: Set_PEV_Sensitivity(%d) failed", pevSensitivity);
    }
  }

  testWaitForTimeout(1000);
}

/*
 * Turn_OFF_OWD()
 * --------------
 * Disables Continuous Driving Recording (OWD) via panel LED toggle.
 * Retries up to 15 times while BLTN_CAM_RecSet_OWD is still 0x2 (ON).
 *
 * Keywords: OWD off, disable driving recording, turn off continuous driving,
 *           BLTN_CAM_RecSet_OWD, panel LED_Ctrl_SetOWD
 * Panel variable: @panel::LED_Ctrl_SetOWD = 1 (toggle click)
 * Signal checked: BLTN_CAM_RecSet_OWD == 0x1 (OFF)
 * Retry: up to 15 attempts, 2000ms between each
 * Return: void (logs SUCCESS/FAILED)
 */
void Turn_OFF_OWD()
{
  int retry = 0;
  int is_off = 0;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;

  write("=== Turn OFF OWD via panel ===");

  is_off = Check_RecSet_OWD_OFF();
  if (is_off == 1)
  {
    write(">>> SET OK: OWD already OFF (0x1), no action required <<<");
    return;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  is_off = Check_RecSet_OWD_OFF();
  while (is_off == 0 && retry < 15)
  {
    write("  Attempt %d: Force OWD ON edge, then OWD OFF...", retry + 1);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_OWD_NvalueSet = 0x2;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_OWD_NvalueSet = 0x2");
    testWaitForTimeout(500);

    @panel::LED_Ctrl_SetOWD = 0;
    testWaitForTimeout(100);
    @panel::LED_Ctrl_SetOWD = 1;
    testWaitForTimeout(200);
    @panel::LED_Ctrl_SetOWD = 0;
    testWaitForTimeout(500);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_OWD_NvalueSet = 0x1;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_OWD_NvalueSet = 0x1");

    testWaitForTimeout(3000);
    is_off = Check_RecSet_OWD_OFF();
    retry++;
  }

  if (is_off == 1)
    write(">>> SUCCESS: OWD = OFF (0x1) <<<");
  else
    write(">>> FAILED: Could not set OWD to OFF <<<");

  Exit_BLTN_CAM();
  testWaitForTimeout(1000);
}

/*
 * Turn_OFF_OWP()
 * --------------
 * Disables Continuous Parking Recording (OWP) via panel LED toggle.
 * Retries up to 15 times while BLTN_CAM_RecSet_OWP is still 0x2 (ON).
 *
 * Keywords: OWP off, disable parking recording, turn off continuous parking,
 *           BLTN_CAM_RecSet_OWP, panel LED_Ctrl_SetOWP
 * Panel variable: @panel::LED_Ctrl_SetOWP = 1
 * Signal checked: BLTN_CAM_RecSet_OWP == 0x1 (OFF)
 * Retry: up to 15 attempts, 2000ms between each
 * Return: void (logs SUCCESS/FAILED)
 */
void Turn_OFF_OWP()
{
  int retry = 0;
  int is_off = 0;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;

  write("=== Turn OFF OWP via panel ===");

  is_off = Check_RecSet_OWP_OFF();
  if (is_off == 1)
  {
    write(">>> SET OK: OWP already OFF (0x1), no action required <<<");
    return;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  is_off = Check_RecSet_OWP_OFF();
  while (is_off == 0 && retry < 15)
  {
    write("  Attempt %d: Force OWP ON edge, then OWP OFF...", retry + 1);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_OWP_NvalueSet = 0x2;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_OWP_NvalueSet = 0x2");
    testWaitForTimeout(500);

    @panel::LED_Ctrl_SetOWP = 0;
    testWaitForTimeout(100);
    @panel::LED_Ctrl_SetOWP = 1;
    testWaitForTimeout(200);
    @panel::LED_Ctrl_SetOWP = 0;
    testWaitForTimeout(500);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_OWP_NvalueSet = 0x1;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_OWP_NvalueSet = 0x1");

    testWaitForTimeout(3000);
    is_off = Check_RecSet_OWP_OFF();
    retry++;
  }

  if (is_off == 1)
    write(">>> SUCCESS: OWP = OFF (0x1) <<<");
  else
    write(">>> FAILED: Could not set OWP to OFF <<<");

  Exit_BLTN_CAM();
  testWaitForTimeout(1000);
}

/*
 * Turn_OFF_DEV()
 * --------------
 * Disables Driving Event Recording (DEV) via panel LED toggle.
 * Retries up to 15 times while BLTN_CAM_RecSet_DEV is still 0x2 (ON).
 *
 * Keywords: DEV off, disable driving event, turn off event recording,
 *           BLTN_CAM_RecSet_DEV, panel LED_Ctrl_SetDEV
 * Panel variable: @panel::LED_Ctrl_SetDEV = 1
 * Signal checked: BLTN_CAM_RecSet_DEV == 0x1 (OFF)
 * Retry: up to 15 attempts, 3000ms between each
 * Return: void (logs SUCCESS/FAILED)
 */
void Turn_OFF_DEV()
{
  int retry = 0;
  int is_off = 0;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;

  write("=== Turn OFF DEV via panel ===");

  is_off = Check_RecSet_DEV_OFF();
  if (is_off == 1)
  {
    write(">>> SET OK: DEV already OFF (0x1), no action required <<<");
    return;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  is_off = Check_RecSet_DEV_OFF();
  while (is_off == 0 && retry < 15)
  {
    write("  Attempt %d: Force DEV ON edge, then DEV OFF...", retry + 1);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_DEV_NvalueSet = 0x2;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_DEV_NvalueSet = 0x2");
    testWaitForTimeout(500);

    @panel::LED_Ctrl_SetDEV = 0;
    testWaitForTimeout(100);
    @panel::LED_Ctrl_SetDEV = 1;
    testWaitForTimeout(200);
    @panel::LED_Ctrl_SetDEV = 0;
    testWaitForTimeout(500);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_DEV_NvalueSet = 0x1;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_DEV_NvalueSet = 0x1");

    testWaitForTimeout(3000);
    is_off = Check_RecSet_DEV_OFF();
    retry++;
  }

  if (is_off == 1)
    write(">>> SUCCESS: DEV = OFF (0x1) <<<");
  else
    write(">>> FAILED: Could not set DEV to OFF <<<");

  Exit_BLTN_CAM();
  testWaitForTimeout(1000);
}

/*
 * Turn_OFF_PEV()
 * --------------
 * Disables Parking Event Recording (PEV) via panel LED toggle.
 * Retries up to 15 times while BLTN_CAM_RecSet_PEV is still 0x2 (ON).
 *
 * Keywords: PEV off, disable parking event, turn off parking event recording,
 *           BLTN_CAM_RecSet_PEV, panel LED_Ctrl_SetPEV
 * Panel variable: @panel::LED_Ctrl_SetPEV = 1
 * Signal checked: BLTN_CAM_RecSet_PEV == 0x1 (OFF)
 * Retry: up to 15 attempts, 2000ms between each
 * Return: void (logs SUCCESS/FAILED)
 */
void Turn_OFF_PEV()
{
  int retry = 0;
  int is_off = 0;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;

  write("=== Turn OFF PEV via panel ===");

  is_off = Check_RecSet_PEV_OFF();
  if (is_off == 1)
  {
    write(">>> SET OK: PEV already OFF (0x1), no action required <<<");
    return;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  is_off = Check_RecSet_PEV_OFF();
  while (is_off == 0 && retry < 15)
  {
    write("  Attempt %d: Force PEV ON edge, then PEV OFF...", retry + 1);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_PEV_NvalueSet = 0x2;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_PEV_NvalueSet = 0x2");
    testWaitForTimeout(500);

    @panel::LED_Ctrl_SetPEV = 0;
    testWaitForTimeout(100);
    @panel::LED_Ctrl_SetPEV = 1;
    testWaitForTimeout(200);
    @panel::LED_Ctrl_SetPEV = 0;
    testWaitForTimeout(500);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Rec_PEV_NvalueSet = 0x1;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_Rec_PEV_NvalueSet = 0x1");

    testWaitForTimeout(3000);
    is_off = Check_RecSet_PEV_OFF();
    retry++;
  }

  if (is_off == 1)
    write(">>> SUCCESS: PEV = OFF (0x1) <<<");
  else
    write(">>> FAILED: Could not set PEV to OFF <<<");

  Exit_BLTN_CAM();
  testWaitForTimeout(1000);
}

// =============================================================================
// SECTION 5: RECORDING STATUS CHECK (Instant Verification)
// =============================================================================
//
// Purpose: Instantly read and verify recording-related CAN signals.
//          No retry or polling - just reads the current signal value once.
//          Used for test step verification (PASS/FAIL reporting).
// Keywords: check, verify, instant check, recording status, signal read,
//           RecSet, RecSta, PASS, FAIL, test verification, assertion
//
// Two signal categories:
//
//   RecSet (Recording Setting) - whether the recording function is configured:
//     BLTN_CAM_RecSet_OWD / OWP / DEV / PEV / PMD
//     Values: 0x1 = OFF (disabled), 0x2 = ON (enabled)
//     Functions: Check_RecSet_XXX_ON() / Check_RecSet_XXX_OFF()
//
//   RecSta (Recording Status) - whether recording is actually active:
//     BLTN_CAM_RecSta_OWD / OWP / DEV / TML / MAR / PEV / PMD
//     Values: 0x0 = OFF (not recording), 0x1 = Recording (actively recording)
//     Functions: Check_RecSta_XXX_Recording() / Check_RecSta_XXX_OFF()
//
// Return: 1 = PASS (signal matches expected), 0 = FAIL (mismatch)
// =============================================================================

// ---------------------------------------------------------------------------
// RecSet checks: verify recording SETTING status (0x1=OFF, 0x2=ON)
// Keywords: RecSet, recording setting, configuration check, ON/OFF setting
// ---------------------------------------------------------------------------

/*
 * Check_RecSet_OWD_ON() / Check_RecSet_OWD_OFF()
 * Check if OWD (Continuous Driving Recording) setting is ON (0x2) or OFF (0x1).
 * Signal: BLTN_CAM_RecSet_OWD
 * Keywords: OWD setting, driving recording setting, RecSet OWD
 */
int Check_RecSet_OWD_ON()
{
  byte val;
  val = $BLTN_CAM_RecSet_OWD;
  write("  BLTN_CAM_RecSet_OWD = 0x%X", val);
  if (val == 0x2)
  {
    write(">>> PASS: RecSet_OWD = ON <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSet_OWD is NOT ON <<<");
    return 0;
  }
}

int Check_RecSet_OWD_OFF()
{
  byte val;
  val = $BLTN_CAM_RecSet_OWD;
  write("  BLTN_CAM_RecSet_OWD = 0x%X", val);
  if (val == 0x1)
  {
    write(">>> PASS: RecSet_OWD = OFF <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSet_OWD is NOT OFF <<<");
    return 0;
  }
}

/*
 * Check_RecSet_OWP_ON() / Check_RecSet_OWP_OFF()
 * Check if OWP (Continuous Parking Recording) setting is ON (0x2) or OFF (0x1).
 * Signal: BLTN_CAM_RecSet_OWP
 * Keywords: OWP setting, parking recording setting, RecSet OWP
 */
int Check_RecSet_OWP_ON()
{
  byte val;
  val = $BLTN_CAM_RecSet_OWP;
  write("  BLTN_CAM_RecSet_OWP = 0x%X", val);
  if (val == 0x2)
  {
    write(">>> PASS: RecSet_OWP = ON <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSet_OWP is NOT ON <<<");
    return 0;
  }
}

int Check_RecSet_OWP_OFF()
{
  byte val;
  val = $BLTN_CAM_RecSet_OWP;
  write("  BLTN_CAM_RecSet_OWP = 0x%X", val);
  if (val == 0x1)
  {
    write(">>> PASS: RecSet_OWP = OFF <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSet_OWP is NOT OFF <<<");
    return 0;
  }
}

/*
 * Check_RecSet_DEV_ON() / Check_RecSet_DEV_OFF()
 * Check if DEV (Driving Event Recording) setting is ON (0x2) or OFF (0x1).
 * Signal: BLTN_CAM_RecSet_DEV
 * Keywords: DEV setting, driving event setting, RecSet DEV
 */
int Check_RecSet_DEV_ON()
{
  byte val;
  val = $BLTN_CAM_RecSet_DEV;
  write("  BLTN_CAM_RecSet_DEV = 0x%X", val);
  if (val == 0x2)
  {
    write(">>> PASS: RecSet_DEV = ON <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSet_DEV is NOT ON <<<");
    return 0;
  }
}

int Check_RecSet_DEV_OFF()
{
  byte val;
  val = $BLTN_CAM_RecSet_DEV;
  write("  BLTN_CAM_RecSet_DEV = 0x%X", val);
  if (val == 0x1)
  {
    write(">>> PASS: RecSet_DEV = OFF <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSet_DEV is NOT OFF <<<");
    return 0;
  }
}

/*
 * Check_RecSet_PEV_ON() / Check_RecSet_PEV_OFF()
 * Check if PEV (Parking Event Recording) setting is ON (0x2) or OFF (0x1).
 * Signal: BLTN_CAM_RecSet_PEV
 * Keywords: PEV setting, parking event setting, RecSet PEV
 */
int Check_RecSet_PEV_ON()
{
  byte val;
  val = $BLTN_CAM_RecSet_PEV;
  write("  BLTN_CAM_RecSet_PEV = 0x%X", val);
  if (val == 0x2)
  {
    write(">>> PASS: RecSet_PEV = ON <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSet_PEV is NOT ON <<<");
    return 0;
  }
}

int Check_RecSet_PEV_OFF()
{
  byte val;
  val = $BLTN_CAM_RecSet_PEV;
  write("  BLTN_CAM_RecSet_PEV = 0x%X", val);
  if (val == 0x1)
  {
    write(">>> PASS: RecSet_PEV = OFF <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSet_PEV is NOT OFF <<<");
    return 0;
  }
}

// ---------------------------------------------------------------------------
// RecSta checks: verify recording ACTIVE status (0x0=OFF, 0x1=Recording)
// Keywords: RecSta, recording status, active recording, is recording,
//           recording in progress, actual recording state
// ---------------------------------------------------------------------------

/*
 * Check_RecSta_OWD_Recording() / Check_RecSta_OWD_OFF()
 * Check if OWD is actively recording (0x1) or stopped (0x0).
 * Signal: BLTN_CAM_RecSta_OWD
 * Keywords: OWD status, driving recording active, OWD RecSta
 */
int Check_RecSta_OWD_Recording()
{
  byte val;
  val = $BLTN_CAM_RecSta_OWD;
  write("  BLTN_CAM_RecSta_OWD = 0x%X", val);
  if (val == 0x1)
  {
    write(">>> PASS: RecSta_OWD = Recording <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSta_OWD is NOT Recording <<<");
    return 0;
  }
}

int Check_RecSta_OWD_OFF()
{
  byte val;
  val = $BLTN_CAM_RecSta_OWD;
  write("  BLTN_CAM_RecSta_OWD = 0x%X", val);
  if (val == 0x0)
  {
    write(">>> PASS: RecSta_OWD = OFF <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSta_OWD is NOT OFF <<<");
    return 0;
  }
}

/*
 * Check_RecSta_OWP_Recording() / Check_RecSta_OWP_OFF()
 * Check if OWP is actively recording (0x1) or stopped (0x0).
 * Signal: BLTN_CAM_RecSta_OWP
 * Keywords: OWP status, parking recording active, OWP RecSta
 */
int Check_RecSta_OWP_Recording()
{
  byte val;
  val = $BLTN_CAM_RecSta_OWP;
  write("  BLTN_CAM_RecSta_OWP = 0x%X", val);
  if (val == 0x1)
  {
    write(">>> PASS: RecSta_OWP = Recording <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSta_OWP is NOT Recording <<<");
    return 0;
  }
}

int Check_RecSta_OWP_OFF()
{
  byte val;
  val = $BLTN_CAM_RecSta_OWP;
  write("  BLTN_CAM_RecSta_OWP = 0x%X", val);
  if (val == 0x0)
  {
    write(">>> PASS: RecSta_OWP = OFF <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSta_OWP is NOT OFF <<<");
    return 0;
  }
}

/*
 * Check_RecSta_DEV_Recording() / Check_RecSta_DEV_OFF()
 * Check if DEV is actively recording (0x1) or stopped (0x0).
 * Signal: BLTN_CAM_RecSta_DEV
 * Keywords: DEV status, driving event recording active, DEV RecSta
 */
int Check_RecSta_DEV_Recording()
{
  byte val;
  val = $BLTN_CAM_RecSta_DEV;
  write("  BLTN_CAM_RecSta_DEV = 0x%X", val);
  if (val == 0x1)
  {
    write(">>> PASS: RecSta_DEV = Recording <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSta_DEV is NOT Recording <<<");
    return 0;
  }
}

int Check_RecSta_DEV_OFF()
{
  byte val;
  val = $BLTN_CAM_RecSta_DEV;
  write("  BLTN_CAM_RecSta_DEV = 0x%X", val);
  if (val == 0x0)
  {
    write(">>> PASS: RecSta_DEV = OFF <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSta_DEV is NOT OFF <<<");
    return 0;
  }
}

/*
 * Check_RecSta_TML_Recording() / Check_RecSta_TML_OFF()
 * Check if TML (Time-lapse) is actively recording (0x1) or stopped (0x0).
 * Signal: BLTN_CAM_RecSta_TML
 * Keywords: TML status, timelapse recording active, TML RecSta
 */
int Check_RecSta_TML_Recording()
{
  byte val;
  val = $BLTN_CAM_RecSta_TML;
  write("  BLTN_CAM_RecSta_TML = 0x%X", val);
  if (val == 0x1)
  {
    write(">>> PASS: RecSta_TML = Recording <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSta_TML is NOT Recording <<<");
    return 0;
  }
}

int Check_RecSta_TML_OFF()
{
  byte val;
  val = $BLTN_CAM_RecSta_TML;
  write("  BLTN_CAM_RecSta_TML = 0x%X", val);
  if (val == 0x0)
  {
    write(">>> PASS: RecSta_TML = OFF <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSta_TML is NOT OFF <<<");
    return 0;
  }
}

/*
 * Check_RecSta_MAR_Recording() / Check_RecSta_MAR_OFF()
 * Check if MAR (Manual Recording) is actively recording (0x1) or stopped (0x0).
 * Signal: BLTN_CAM_RecSta_MAR
 * Keywords: MAR status, manual recording active, MAR RecSta, manual event
 */
int Check_RecSta_MAR_Recording()
{
  byte val;
  val = $BLTN_CAM_RecSta_MAR;
  write("  BLTN_CAM_RecSta_MAR = 0x%X", val);
  if (val == 0x1)
  {
    write(">>> PASS: RecSta_MAR = Recording <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSta_MAR is NOT Recording <<<");
    return 0;
  }
}

int Check_RecSta_MAR_OFF()
{
  byte val;
  val = $BLTN_CAM_RecSta_MAR;
  write("  BLTN_CAM_RecSta_MAR = 0x%X", val);
  if (val == 0x0)
  {
    write(">>> PASS: RecSta_MAR = OFF <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSta_MAR is NOT OFF <<<");
    return 0;
  }
}

/*
 * Check_RecSta_PEV_Recording() / Check_RecSta_PEV_OFF()
 * Check if PEV (Parking Event) is actively recording (0x1) or stopped (0x0).
 * Signal: BLTN_CAM_RecSta_PEV
 * Keywords: PEV status, parking event recording active, PEV RecSta
 */
int Check_RecSta_PEV_Recording()
{
  byte val;
  val = $BLTN_CAM_RecSta_PEV;
  write("  BLTN_CAM_RecSta_PEV = 0x%X", val);
  if (val == 0x1)
  {
    write(">>> PASS: RecSta_PEV = Recording <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSta_PEV is NOT Recording <<<");
    return 0;
  }
}

int Check_RecSta_PEV_OFF()
{
  byte val;
  val = $BLTN_CAM_RecSta_PEV;
  write("  BLTN_CAM_RecSta_PEV = 0x%X", val);
  if (val == 0x0)
  {
    write(">>> PASS: RecSta_PEV = OFF <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSta_PEV is NOT OFF <<<");
    return 0;
  }
}

/*
 * Check_All_Recording_Signals()
 * -----------------------------
 * Checks all 4 main recording status signals at once:
 *   OWD, TML, DEV, MAR - all must be 0x1 (Recording) to PASS.
 * Used for tests that enable multiple recordings simultaneously.
 *
 * Keywords: check all recordings, bulk check, all RecSta, multi-signal check,
 *           OWD TML DEV MAR, all recording active, combined verification
 * Signals checked: RecSta_OWD, RecSta_TML, RecSta_DEV, RecSta_MAR
 * Return: 1 = ALL are Recording, 0 = at least one is NOT Recording
 */
int Check_All_Recording_Signals()
{
  int result = 1;
  byte rec_owd, rec_tml, rec_dev, rec_mar;

  write("=== Checking all 4 recording signals ===");

  rec_owd = $BLTN_CAM_RecSta_OWD;
  rec_tml = $BLTN_CAM_RecSta_TML;
  rec_dev = $BLTN_CAM_RecSta_DEV;
  rec_mar = $BLTN_CAM_RecSta_MAR;

  write("  BLTN_CAM_RecSta_OWD = 0x%X", rec_owd);
  write("  BLTN_CAM_RecSta_TML = 0x%X", rec_tml);
  write("  BLTN_CAM_RecSta_DEV = 0x%X", rec_dev);
  write("  BLTN_CAM_RecSta_MAR = 0x%X", rec_mar);

  if (rec_owd != 0x1)
  {
    write(">>> FAIL: RecSta_OWD is NOT Recording <<<");
    result = 0;
  }
  if (rec_tml != 0x1)
  {
    write(">>> FAIL: RecSta_TML is NOT Recording <<<");
    result = 0;
  }
  if (rec_dev != 0x1)
  {
    write(">>> FAIL: RecSta_DEV is NOT Recording <<<");
    result = 0;
  }
  if (rec_mar != 0x1)
  {
    write(">>> FAIL: RecSta_MAR is NOT Recording <<<");
    result = 0;
  }

  if (result == 1)
    write(">>> PASS: All 4 recording signals = 0x1 (Recording) <<<");

  return result;
}

/*
 * Check_SWVerMinor3(int expectedVersion)
 * --------------------------------------
 * Checks BLTN_CAM_SWVerMinor3 against expected version and returns 1/0.
 *
 * Notes:
 *   - Compares as integer value, so display format like 01 is treated as 1.
 *   - Example: signal=01 and expectedVersion=1 -> PASS.
 *
 * Parameter: expectedVersion - expected minor version (0..99)
 * Return: 1 = match, 0 = mismatch
 */
int Check_SWVerMinor3(int expectedVersion)
{
  int actualVersion;
  int actualUnit;
  int expectedUnit;

  actualVersion = $BLTN_CAM_SWVerMinor3;
  actualUnit = actualVersion % 10;
  expectedUnit = expectedVersion % 10;

  write("  BLTN_CAM_SWVerMinor3 = %02d (unit=%d), expected=%d (unit=%d)",
        actualVersion,
        actualUnit,
        expectedVersion,
        expectedUnit);

  if (actualUnit == expectedUnit)
  {
    write(">>> PASS: BLTN_CAM_SWVerMinor3 unit digit matched expected <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: BLTN_CAM_SWVerMinor3 unit digit mismatched expected <<<");
    return 0;
  }
}

// =============================================================================
// SECTION 6: RECORDING STATUS WAIT (Polling with Timeout)
// =============================================================================
//
// Purpose: Wait for a specific recording to start (RecSta = 0x1) with a
//          configurable timeout. Uses polling loop with periodic status
//          logging. Unlike Section 5 (instant check), these functions keep
//          trying until timeout expires.
// Keywords: wait for recording, polling, timeout, RecSta, recording start,
//           wait until recording, long wait, recording detection, elapsed time
//
// All functions follow the same pattern:
//   - Poll the RecSta signal every 2 seconds
//   - Log current status at ~1/6 of total timeout interval
//   - Return 1 immediately when recording detected
//   - Return 0 if timeout reached without detection
//
// Parameter: timeout_ms - maximum wait time in milliseconds
// Return: 1 = recording detected within timeout, 0 = timeout (recording NOT
// detected)
// =============================================================================

/*
 * WaitFor_RecSta_OWD_Recording(long timeout_ms)
 * Wait for OWD (Continuous Driving) recording to start.
 * Signal: BLTN_CAM_RecSta_OWD == 0x1
 * Keywords: wait OWD, OWD recording start, driving recording wait
 */
int WaitFor_RecSta_OWD_Recording(long timeout_ms)
{
  long elapsed = 0;
  long log_interval;

  write("=== Waiting for OWD Recording (max %d seconds) ===",
        timeout_ms / 1000);

  log_interval = (timeout_ms / 6 / 2000) * 2000;
  if (log_interval < 2000)
    log_interval = 2000;

  while (elapsed < timeout_ms)
  {
    if ($BLTN_CAM_RecSta_OWD == 0x1)
    {
      write(">>> DETECTED: OWD Recording ON after %d seconds <<<",
            elapsed / 1000);
      testWaitForTimeout(1000);
      return 1;
    }
    if (elapsed == 0 || elapsed % log_interval == 0)
      write("  [%d s] RecSta_OWD=0x%X", elapsed / 1000, $BLTN_CAM_RecSta_OWD);

    testWaitForTimeout(2000);
    elapsed += 2000;
  }
  write(">>> TIMEOUT: OWD Recording NOT detected <<<");
  return 0;
}

/*
 * WaitFor_RecSta_OWP_Recording(long timeout_ms)
 * Wait for OWP (Continuous Parking) recording to start.
 * Signal: BLTN_CAM_RecSta_OWP == 0x1
 * Keywords: wait OWP, OWP recording start, parking recording wait
 */
int WaitFor_RecSta_OWP_Recording(long timeout_ms)
{
  long elapsed = 0;
  long log_interval;

  write("=== Waiting for OWP Recording (max %d seconds) ===",
        timeout_ms / 1000);

  log_interval = (timeout_ms / 6 / 2000) * 2000;
  if (log_interval < 2000)
    log_interval = 2000;

  while (elapsed < timeout_ms)
  {
    if ($BLTN_CAM_RecSta_OWP == 0x1)
    {
      write(">>> DETECTED: OWP Recording ON after %d seconds <<<",
            elapsed / 1000);
      return 1;
    }
    if (elapsed == 0 || elapsed % log_interval == 0)
      write("  [%d s] RecSta_OWP=0x%X", elapsed / 1000, $BLTN_CAM_RecSta_OWP);

    testWaitForTimeout(2000);
    elapsed += 2000;
  }
  write(">>> TIMEOUT: OWP Recording NOT detected <<<");
  return 0;
}

/*
 * WaitFor_RecSta_DEV_Recording(long timeout_s)
 * Prompt user to physically trigger DEV event, then wait for DEV recording.
 * Signal: BLTN_CAM_RecSta_DEV == 0x1
 * Keywords: wait DEV, DEV recording start, driving event recording wait,
 * trigger
 */
int WaitFor_RecSta_DEV_Recording(long timeout_s)
{
  long i;
  long trigger_attempt;

  write("");
  write("*************************************************************");
  write("");
  write("*** [ACTION REQUIRED] TAC DONG SU KIEN DEV NGAY BAY GIO! ***");
  SetLogMessage("TAC DONG SU KIEN DEV NGAY BAY GIO!");
  write("");
  write("*************************************************************");
  write("");
  for (trigger_attempt = 1; trigger_attempt <= 5; trigger_attempt++)
  {
    write("  Sending DEV COM trigger sequence %d/5...", trigger_attempt);
    Send_Value_To_COM(8);
    testWaitForTimeout(500);
    Send_Value_To_COM(8);
    testWaitForTimeout(500);
    Send_Value_To_COM(8);
    testWaitForTimeout(500);
    Send_Value_To_COM(8);

    if ($BLTN_CAM_RecSta_DEV == 0x1)
    {
      write(">>> [OK] BLTN_CAM_RecSta_DEV = 0x1 (ON) after trigger %d/5 <<<",
            trigger_attempt);
      SetLogMessage("");
      return 1;
    }
  }

  write("  Waiting for BLTN_CAM_RecSta_DEV = 0x1 (ON) (max %ds)...", timeout_s);
  for (i = 0; i < timeout_s; i++)
  {
    if ($BLTN_CAM_RecSta_DEV == 0x1)
    {
      write(">>> [OK] BLTN_CAM_RecSta_DEV = 0x1 (ON) after %ds <<<", i);
      SetLogMessage("");
      return 1;
    }
    testWaitForTimeout(1000);
  }
  write(">>> [NG] Timeout %ds! BLTN_CAM_RecSta_DEV = 0x%X (expected 0x1) <<<",
        timeout_s, $BLTN_CAM_RecSta_DEV);
  SetLogMessage("");
  return 0;
}

/*
 * Check_Monitor_RecSta_Signal_AlwaysOFF(int signal_id, long timeout_ms)
 * -----------------------------------------------------------------------
 * Monitors a recording status (RecSta) signal for a specified duration
 * and verifies it ALWAYS remains 0x0 (OFF / not recording).
 * Logs a WARNING if recording is detected at any point during monitoring.
 *
 * Signal IDs:
 *   1 = OWD (BLTN_CAM_RecSta_OWD)   2 = OWP (BLTN_CAM_RecSta_OWP)
 *   3 = DEV (BLTN_CAM_RecSta_DEV)   4 = PEV (BLTN_CAM_RecSta_PEV)
 *   5 = MAR (BLTN_CAM_RecSta_MAR)   6 = TML (BLTN_CAM_RecSta_TML)
 *   7 = PMD (BLTN_CAM_RecSta_PMD)
 *
 * Keywords: monitor always off, stability check, signal always off, RecSta 0x0,
 *           OWD OWP DEV PEV MAR TML PMD monitor, recording suppressed
 * Parameters:
 *   signal_id  - ID of the RecSta signal to monitor (1~7, see table above)
 *   timeout_ms - duration to monitor in milliseconds
 * Return: 1 = always OFF for full duration (PASS), 0 = recording detected
 * (FAIL)
 */
int Check_Monitor_RecSta_Signal_AlwaysOFF(int signal_id, long timeout_ms)
{
  long elapsed = 0;
  long check_interval = 1000;
  byte val = 0;
  int always_off = 1;
  long log_interval;
  char sig_name[20];

  if (signal_id == 1)
    snprintf(sig_name, elcount(sig_name), "%s", "RecSta_OWD");
  else if (signal_id == 2)
    snprintf(sig_name, elcount(sig_name), "%s", "RecSta_OWP");
  else if (signal_id == 3)
    snprintf(sig_name, elcount(sig_name), "%s", "RecSta_DEV");
  else if (signal_id == 4)
    snprintf(sig_name, elcount(sig_name), "%s", "RecSta_PEV");
  else if (signal_id == 5)
    snprintf(sig_name, elcount(sig_name), "%s", "RecSta_MAR");
  else if (signal_id == 6)
    snprintf(sig_name, elcount(sig_name), "%s", "RecSta_TML");
  else if (signal_id == 7)
    snprintf(sig_name, elcount(sig_name), "%s", "RecSta_PMD");
  else
  {
    write(">>> ERROR: Invalid signal_id %d (must be 1~7) <<<", signal_id);
    return 0;
  }

  write("=== Monitoring %s for %d seconds (expected: always 0x0 OFF) ===",
        sig_name, timeout_ms / 1000);

  log_interval = (timeout_ms / 6 / check_interval) * check_interval;
  if (log_interval < check_interval)
    log_interval = check_interval;

  while (elapsed < timeout_ms)
  {
    if (signal_id == 1)
      val = $BLTN_CAM_RecSta_OWD;
    else if (signal_id == 2)
      val = $BLTN_CAM_RecSta_OWP;
    else if (signal_id == 3)
      val = $BLTN_CAM_RecSta_DEV;
    else if (signal_id == 4)
      val = $BLTN_CAM_RecSta_PEV;
    else if (signal_id == 5)
      val = $BLTN_CAM_RecSta_MAR;
    else if (signal_id == 6)
      val = $BLTN_CAM_RecSta_TML;
    else if (signal_id == 7)
      val = $BLTN_CAM_RecSta_PMD;

    if (elapsed == 0 || elapsed % log_interval == 0)
      write("  [%d s / %d s] BLTN_CAM_%s = 0x%X", elapsed / 1000,
            timeout_ms / 1000, sig_name, val);

    if (val != 0x0)
    {
      write(">>> WARNING [%d s]: BLTN_CAM_%s changed to 0x%X (not OFF) <<<",
            elapsed / 1000, sig_name, val);
      always_off = 0;
    }

    testWaitForTimeout(check_interval);
    elapsed += check_interval;
  }

  if (always_off == 1)
    write(">>> PASS: BLTN_CAM_%s remained 0x0 (OFF) for %d seconds <<<",
          sig_name, timeout_ms / 1000);
  else
    write(">>> FAIL: BLTN_CAM_%s was NOT always OFF <<<", sig_name);

  return always_off;
}

/*
 * Check_Monitor_RecSta_Signal_AlwaysON(int signal_id, long timeout_ms)
 * -----------------------------------------------------------------------
 * Monitors a recording status (RecSta) signal for a specified duration
 * and verifies it ALWAYS remains 0x1 (Recording / ON).
 * Logs a WARNING if recording STOPS at any point during monitoring.
 *
 * Signal IDs:
 *   1 = OWD (BLTN_CAM_RecSta_OWD)   2 = OWP (BLTN_CAM_RecSta_OWP)
 *   3 = DEV (BLTN_CAM_RecSta_DEV)   4 = PEV (BLTN_CAM_RecSta_PEV)
 *   5 = MAR (BLTN_CAM_RecSta_MAR)   6 = TML (BLTN_CAM_RecSta_TML)
 *   7 = PMD (BLTN_CAM_RecSta_PMD)
 *
 * Keywords: monitor always on, stability check, signal always recording, RecSta
 * 0x1, OWD OWP DEV PEV MAR TML PMD monitor, continuous recording check
 * Parameters:
 *   signal_id  - ID of the RecSta signal to monitor (1~7, see table above)
 *   timeout_ms - duration to monitor in milliseconds
 * Return: 1 = always ON for full duration (PASS), 0 = recording interrupted
 * (FAIL)
 */
int Check_Monitor_RecSta_Signal_AlwaysON(int signal_id, long timeout_ms)
{
  long elapsed = 0;
  long check_interval = 1000;
  byte val = 0;
  int always_on = 1;
  long log_interval;
  char sig_name[20];

  if (signal_id == 1)
    snprintf(sig_name, elcount(sig_name), "%s", "RecSta_OWD");
  else if (signal_id == 2)
    snprintf(sig_name, elcount(sig_name), "%s", "RecSta_OWP");
  else if (signal_id == 3)
    snprintf(sig_name, elcount(sig_name), "%s", "RecSta_DEV");
  else if (signal_id == 4)
    snprintf(sig_name, elcount(sig_name), "%s", "RecSta_PEV");
  else if (signal_id == 5)
    snprintf(sig_name, elcount(sig_name), "%s", "RecSta_MAR");
  else if (signal_id == 6)
    snprintf(sig_name, elcount(sig_name), "%s", "RecSta_TML");
  else if (signal_id == 7)
    snprintf(sig_name, elcount(sig_name), "%s", "RecSta_PMD");
  else
  {
    write(">>> ERROR: Invalid signal_id %d (must be 1~7) <<<", signal_id);
    return 0;
  }

  write("=== Monitoring %s for %d seconds (expected: always 0x1 Recording) ===",
        sig_name, timeout_ms / 1000);

  log_interval = (timeout_ms / 6 / check_interval) * check_interval;
  if (log_interval < check_interval)
    log_interval = check_interval;

  while (elapsed < timeout_ms)
  {
    if (signal_id == 1)
      val = $BLTN_CAM_RecSta_OWD;
    else if (signal_id == 2)
      val = $BLTN_CAM_RecSta_OWP;
    else if (signal_id == 3)
      val = $BLTN_CAM_RecSta_DEV;
    else if (signal_id == 4)
      val = $BLTN_CAM_RecSta_PEV;
    else if (signal_id == 5)
      val = $BLTN_CAM_RecSta_MAR;
    else if (signal_id == 6)
      val = $BLTN_CAM_RecSta_TML;
    else if (signal_id == 7)
      val = $BLTN_CAM_RecSta_PMD;

    if (elapsed == 0 || elapsed % log_interval == 0)
      write("  [%d s / %d s] BLTN_CAM_%s = 0x%X", elapsed / 1000,
            timeout_ms / 1000, sig_name, val);

    if (val != 0x1)
    {
      write(
          ">>> WARNING [%d s]: BLTN_CAM_%s changed to 0x%X (not Recording) <<<",
          elapsed / 1000, sig_name, val);
      always_on = 0;
    }

    testWaitForTimeout(check_interval);
    elapsed += check_interval;
  }

  if (always_on == 1)
    write(">>> PASS: BLTN_CAM_%s remained 0x1 (Recording) for %d seconds <<<",
          sig_name, timeout_ms / 1000);
  else
    write(">>> FAIL: BLTN_CAM_%s was NOT always Recording <<<", sig_name);

  return always_on;
}

/*
 * Monitor_RecSta_DEV_AlwaysOFF(long timeout_ms)
 * --------------------------------------------
 * Backward-compatible wrapper around Check_Monitor_RecSta_Signal_AlwaysOFF
 * for DEV signal (signal_id = 3).
 *
 * Keywords: monitor DEV off, stability check, DEV always off, RecSta_DEV 0x0
 * Return: 1 = always OFF (PASS), 0 = recording detected (FAIL)
 */
int Monitor_RecSta_DEV_AlwaysOFF(long timeout_ms)
{
  return Check_Monitor_RecSta_Signal_AlwaysOFF(3, timeout_ms);
}

/*
 * WaitFor_RecSta_MAR_Recording(long timeout_ms)
 * Wait for MAR (Manual Recording) recording to start.
 * Signal: BLTN_CAM_RecSta_MAR == 0x1
 * Keywords: wait MAR, MAR recording start, manual recording wait, manual event
 * wait
 */
int WaitFor_RecSta_MAR_Recording(long timeout_ms)
{
  long elapsed = 0;
  long log_interval;

  write("=== Waiting for MAR Recording (max %d seconds) ===",
        timeout_ms / 1000);

  log_interval = (timeout_ms / 6 / 2000) * 2000;
  if (log_interval < 2000)
    log_interval = 2000;

  while (elapsed < timeout_ms)
  {
    if ($BLTN_CAM_RecSta_MAR == 0x1)
    {
      write(">>> DETECTED: MAR Recording ON after %d seconds <<<",
            elapsed / 1000);
      return 1;
    }
    if (elapsed == 0 || elapsed % log_interval == 0)
      write("  [%d s] RecSta_MAR=0x%X", elapsed / 1000, $BLTN_CAM_RecSta_MAR);

    testWaitForTimeout(2000);
    elapsed += 2000;
  }
  write(">>> TIMEOUT: MAR Recording NOT detected <<<");
  return 0;
}

/*
 * WaitFor_RecSta_TML_Recording(long timeout_ms)
 * Wait for TML (Time-lapse) recording to start.
 * Signal: BLTN_CAM_RecSta_TML == 0x1
 * Keywords: wait TML, TML recording start, timelapse recording wait
 */
int WaitFor_RecSta_TML_Recording(long timeout_ms)
{
  long elapsed = 0;
  long log_interval;

  write("=== Waiting for TML Recording (max %d seconds) ===",
        timeout_ms / 1000);

  log_interval = (timeout_ms / 6 / 2000) * 2000;
  if (log_interval < 2000)
    log_interval = 2000;

  while (elapsed < timeout_ms)
  {
    if ($BLTN_CAM_RecSta_TML == 0x1)
    {
      write(">>> DETECTED: TML Recording ON after %d seconds <<<",
            elapsed / 1000);
      return 1;
    }
    if (elapsed == 0 || elapsed % log_interval == 0)
      write("  [%d s] RecSta_TML=0x%X", elapsed / 1000, $BLTN_CAM_RecSta_TML);

    testWaitForTimeout(2000);
    elapsed += 2000;
  }
  write(">>> TIMEOUT: TML Recording NOT detected <<<");
  return 0;
}

/*
 * WaitFor_RecSta_PEV_Recording(long timeout_s)
 * Prompt user to physically trigger PEV event, then wait for PEV recording.
 * Signal: BLTN_CAM_RecSta_PEV == 0x1
 * Keywords: wait PEV, PEV recording start, parking event recording wait,
 * trigger
 */
int WaitFor_RecSta_PEV_Recording(long timeout_s)
{
  long i;
  SetLogMessage("TAC DONG SU KIEN DEV NGAY BAY GIO!");
  write("");
  write("*************************************************************");
  write("");
  write("*** [ACTION REQUIRED] TAC DONG SU KIEN PEV NGAY BAY GIO! ***");
  write("");
  write("*************************************************************");
  write("");

  /*
    Set_BglrAlrm(
        9); // Example: set parking alarm to level 9 to trigger PEV recording
    testWaitForTimeout(5000);
  */
  Send_Value_To_COM(8); // Example: send command to trigger PEV recording
  write("  Waiting for BLTN_CAM_RecSta_PEV = 0x1 (ON) (max %ds)...", timeout_s);
  for (i = 0; i < timeout_s; i++)
  {
    if ($BLTN_CAM_RecSta_PEV == 0x1)
    {
      write(">>> [OK] BLTN_CAM_RecSta_PEV = 0x1 (ON) after %ds <<<", i);
      SetLogMessage("");
      return 1;
    }
    testWaitForTimeout(1000);
  }
  write(">>> [NG] Timeout %ds! BLTN_CAM_RecSta_PEV = 0x%X (expected 0x1) <<<",
        timeout_s, $BLTN_CAM_RecSta_PEV);
  SetLogMessage("");
  return 0;
}

/*
 * WaitFor_PEV_Recorded_Value(byte expectedValue, long timeoutMs)
 * ---------------------------------------------------------------
 * Waits until BLTN_CAM_PEV_Recorded reaches the expected value
 * within timeout using CANoe signal wait API.
 *
 * Keywords: PEV_Recorded wait, exact value wait, polling, timeout
 * Signal checked: BLTN_CAM_PEV_Recorded
 * Parameters:
 *   expectedValue - target BLTN_CAM_PEV_Recorded value
 *   timeoutMs     - timeout in milliseconds
 * Return: 1 = expected value detected within timeout, 0 = timeout
 */
int WaitFor_PEV_Recorded_Value(byte expectedValue, long timeoutMs)
{
  return testWaitForSignalInRange(BLTN_CAM_PEV_Recorded,
                                  expectedValue,
                                  expectedValue,
                                  timeoutMs) == 1;
}

// =============================================================================
// SECTION 7: FACTORY / CUSTOMER MODE CONTROL (ICU PowerAutoCut)
// =============================================================================
//
// Purpose: Control the ICU (Instrument Cluster Unit) PowerAutoCut mode.
//          This determines the camera ECU's power management behavior:
//          - Factory Mode: camera stays powered for testing (no auto-cutoff)
//          - Dealer Mode: intermediate setting for dealer diagnostics
//          - Customer Mode: normal operation with power auto-cutoff to save
//          battery
// Keywords: factory mode, customer mode, dealer mode, ICU, PowerAutoCut,
//           power management, battery protection, auto power off,
//           ICU_PowerAutoCutModSta, mode switch, production line, end of line
//
// Signal: ICU_PowerAutoCutModSta
//   0x0 = Factory Mode  (no auto power cut - for production testing)
//   0x1 = Dealer Mode   (for dealer service/diagnostics)
//   0x2 = Customer Mode (normal use - auto power cut enabled for battery
//   protection)
//
// Typical test flow:
//   Set_Factory_Mode() -> [run test without power cut interference] ->
//   Set_Customer_Mode()
// =============================================================================

/*
 * Set_PowerAutoCut_Mode(byte target_mode, long timeout_ms)
 * --------------------------------------------------------
 * Sets ICU_PowerAutoCutModSta to the specified mode with retry polling.
 * Keeps trying to set the signal and checks for confirmation within timeout.
 * Re-sends the setSignal command at each log interval to ensure delivery.
 *
 * Keywords: set power mode, PowerAutoCut, ICU mode, retry, confirmation,
 *           factory mode set, customer mode set, dealer mode set
 * Signal: ICU_PowerAutoCutModSta
 * Parameters:
 *   target_mode - 0x0 (Factory), 0x1 (Dealer), or 0x2 (Customer)
 *   timeout_ms  - maximum wait time for mode confirmation
 * Return: 1 = mode set successfully, 0 = timeout
 */
int Set_PowerAutoCut_Mode(byte target_mode, long timeout_ms)
{
  long elapsed = 0;
  long log_interval;
  byte current_mode;
  char mode_name[50];

  if (target_mode == 0)
    snprintf(mode_name, elcount(mode_name), "%s", "Factory Mode");
  else if (target_mode == 1)
    snprintf(mode_name, elcount(mode_name), "%s", "Dealer Mode");
  else if (target_mode == 2)
    snprintf(mode_name, elcount(mode_name), "%s", "Customer Mode");
  else
    snprintf(mode_name, elcount(mode_name), "%s", "Unknown Mode");

  write("Setting PowerAutoCutMode to 0x%X (%s)...", target_mode, mode_name);
  current_mode = $ICU_PowerAutoCutModSta;
  if (current_mode == target_mode)
  {
    write(">>> SET OK: PowerAutoCutMode already 0x%X (%s), no action required <<<",
          current_mode, mode_name);
    return 1;
  }

  setSignal(ICU_PowerAutoCutModSta, target_mode);
  testWaitForTimeout(500);

  log_interval = (timeout_ms / 6 / 1000) * 1000;
  if (log_interval < 1000)
    log_interval = 1000;

  while (elapsed < timeout_ms)
  {
    current_mode = $ICU_PowerAutoCutModSta;
    if (current_mode == target_mode)
    {
      write(">>> SUCCESS: PowerAutoCutMode = 0x%X (%s) <<<", current_mode,
            mode_name);
      return 1;
    }
    if (elapsed == 0 || elapsed % log_interval == 0)
    {
      write("  [%d s] PowerAutoCutMode=0x%X (waiting for 0x%X)", elapsed / 1000,
            current_mode, target_mode);
      setSignal(ICU_PowerAutoCutModSta, target_mode);
    }
    testWaitForTimeout(1000);
    elapsed += 1000;
  }

  write(">>> TIMEOUT: Could not set PowerAutoCutMode to 0x%X <<<", target_mode);
  return 0;
}

/*
 * Verify_PowerAutoCut_Mode(byte expected_mode)
 * ---------------------------------------------
 * Instantly verifies that ICU_PowerAutoCutModSta matches the expected mode.
 * No retry - just reads and compares once.
 *
 * Keywords: verify mode, check power mode, PowerAutoCut verify, mode check,
 *           instant verification, factory mode check, customer mode check
 * Signal read: ICU_PowerAutoCutModSta
 * Parameter: expected_mode - 0x0 (Factory), 0x1 (Dealer), or 0x2 (Customer)
 * Return: 1 = PASS (matches), 0 = FAIL (mismatch)
 */
int Verify_PowerAutoCut_Mode(byte expected_mode)
{
  byte current_mode;
  char mode_name[50];

  current_mode = $ICU_PowerAutoCutModSta;

  if (expected_mode == 0)
    snprintf(mode_name, elcount(mode_name), "%s", "Factory Mode");
  else if (expected_mode == 1)
    snprintf(mode_name, elcount(mode_name), "%s", "Dealer Mode");
  else if (expected_mode == 2)
    snprintf(mode_name, elcount(mode_name), "%s", "Customer Mode");
  else
    snprintf(mode_name, elcount(mode_name), "%s", "Unknown Mode");

  write("Verifying PowerAutoCutMode: Current=0x%X, Expected=0x%X (%s)",
        current_mode, expected_mode, mode_name);

  if (current_mode == expected_mode)
  {
    write(">>> VERIFIED: PowerAutoCutMode = 0x%X (%s) <<<", current_mode,
          mode_name);
    return 1;
  }
  else
  {
    write(">>> FAILED: PowerAutoCutMode = 0x%X, Expected 0x%X <<<",
          current_mode, expected_mode);
    return 0;
  }
}

/*
 * Set_Factory_Mode()
 * ------------------
 * Shortcut to set ICU to Factory Mode (0x0) with 30-second timeout.
 * Factory mode disables auto power cutoff, keeping camera powered
 * indefinitely for test automation.
 *
 * Keywords: factory mode, production test, no power cut, keep alive,
 *           test mode, ICU factory, disable auto power off
 * Calls: Set_PowerAutoCut_Mode(0x0, 30000)
 * Return: 1 = success, 0 = timeout
 */
int Set_Factory_Mode() { return Set_PowerAutoCut_Mode(0x0, 30000); }

/*
 * Set_Dealer_Mode()
 * -----------------
 * Shortcut to set ICU to Dealer Mode (0x1) with 30-second timeout.
 * Dealer mode is used for dealer service/diagnostics.
 *
 * Keywords: dealer mode, service mode, diagnostics, ICU dealer
 * Calls: Set_PowerAutoCut_Mode(0x1, 30000)
 * Return: 1 = success, 0 = timeout
 */
int Set_Dealer_Mode() { return Set_PowerAutoCut_Mode(0x1, 30000); }

/*
 * Set_Customer_Mode()
 * -------------------
 * Shortcut to set ICU to Customer Mode (0x2) with 30-second timeout.
 * Customer mode enables auto power cutoff to protect vehicle battery
 * (normal end-user operation).
 *
 * Keywords: customer mode, normal mode, enable power cut, battery protection,
 *           end user mode, ICU customer, auto power off enabled
 * Calls: Set_PowerAutoCut_Mode(0x2, 30000)
 * Return: 1 = success, 0 = timeout
 */
int Set_Customer_Mode() { return Set_PowerAutoCut_Mode(0x2, 30000); }

// =============================================================================
// SECTION 8: FRS RESET DETECTION (SD Card Format / Recording Data Reset)
// =============================================================================
//
// Purpose: Monitor and trigger SD card formatting / recording data reset.
//          Used to clear all recorded data before starting a clean test.
// Keywords: FRS reset, SD card format, recording reset, data clear,
//           CF_AVN_BLTN_CAM_RecReset, BLTN_CAM_SD_Format_Op_State,
//           format SD card, clear recordings, storage reset, clean state
//
// Signals:
//   CF_AVN_BLTN_CAM_RecReset    : 0x1 = Record Data Reset triggered
//   BLTN_CAM_SD_Format_Op_State : 0x1 = SD card formatting in progress
// =============================================================================

/*
 * Wait_For_FRS_Reset(long timeout_ms)
 * ------------------------------------
 * Monitors CF_AVN_BLTN_CAM_RecReset signal and waits for it to become 0x1
 * (indicating recording data reset has been triggered).
 * Polls every 100ms for precise reset detection.
 *
 * Keywords: wait for reset, FRS reset detection, recording data reset,
 *           CF_AVN_BLTN_CAM_RecReset, reset monitoring, SD format wait
 * Signal checked: CF_AVN_BLTN_CAM_RecReset == 0x1
 * Parameter: timeout_ms - maximum wait time in milliseconds
 * Return: 1 = reset detected, 0 = timeout
 */
int Wait_For_FRS_Reset(long timeout_ms)
{
  long elapsed = 0;
  long log_interval;
  byte reset_signal = 0;

  write("Monitoring CF_AVN_BLTN_CAM_RecReset for reset detection...");
  write("Waiting for value = 0x1 (Record Data Reset) (max %d seconds)",
        timeout_ms / 1000);

  log_interval = (timeout_ms / 6 / 100) * 100;
  if (log_interval < 100)
    log_interval = 100;

  while (elapsed < timeout_ms)
  {
    reset_signal = $CF_AVN_BLTN_CAM_RecReset;

    if (reset_signal == 0x1)
    {
      write(">>> Reset detected! CF_AVN_BLTN_CAM_RecReset = 0x1 <<<");
      return 1;
    }

    if (elapsed == 0 || elapsed % log_interval == 0)
    {
      write("  [%d s] CF_AVN_BLTN_CAM_RecReset=0x%X", elapsed / 1000,
            reset_signal);
    }

    testWaitForTimeout(100);
    elapsed += 100;
  }

  write(">>> TIMEOUT: Reset not detected within %d seconds <<<",
        timeout_ms / 1000);
  return 0;
}

/*
 * Rec_Reset()
 * -----------
 * Performs SD card format by repeatedly clicking the RecReset button
 * until BLTN_CAM_SD_Format_Op_State becomes 0x1 (formatting started).
 * Uses infinite retry loop (no max limit) - will keep clicking until success.
 *
 * Keywords: SD card format, recording reset, format storage, Rec Reset click,
 *           BLTN_CAM_SD_Format_Op_State, force format, persistent reset
 * Panel variable: @panel::Button_RecReset = 1
 * Signal checked: BLTN_CAM_SD_Format_Op_State == 0x1
 * Return: void (completes when format operation begins)
 */
void Rec_Reset()
{
  /*
  write("=== Performing Recording Reset (SD Format) ===");
  while (getSignal(BLTN_CAM_SD_Format_Op_State) != 0x1)
  {
    @panel::Button_RecReset = 1;
    testWaitForTimeout(500);
  }
  write(">>> Recording Reset complete (SD_Format_Op_State = 0x1) <<<");
  */
}

// =============================================================================
// SECTION 9: AIN VOLTAGE (VN1600 Hardware Analog Input)
// =============================================================================
//
// Purpose: Read and check analog input voltage from VN1600 hardware interface.
//          Used to verify power supply levels to the camera ECU.
//          Can detect if the camera is powered (>threshold) or unpowered
//          (<threshold).
// Keywords: AIN, analog input, voltage, VN1600, hardware, power supply,
//           voltage measurement, power check, threshold, battery voltage,
//           ECU power, voltage monitoring, hardware interface
//
// System variable: @sysvar::IO::VN1600_1::AIN (float, unit: Volts)
// =============================================================================

/*
 * Get_AIN_Voltage()
 * -----------------
 * Reads the current AIN (Analog Input) voltage from VN1600 hardware.
 * Returns the voltage as a float value in Volts.
 *
 * Keywords: read voltage, AIN voltage, VN1600 AIN, analog input read,
 *           power supply voltage, measure voltage
 * System variable read: @sysvar::IO::VN1600_1::AIN
 * Return: float - current voltage in Volts (e.g., 12.55)
 */
float Get_AIN_Voltage()
{
  float ain_value = 0.0;
  ain_value = @sysvar::IO::VN1600_1::AIN;
  write("  AIN Voltage: %.2f V", ain_value);
  return ain_value;
}

/*
 * Check_AIN_Below_Threshold(float threshold, long timeout_ms)
 * ------------------------------------------------------------
 * Waits for AIN voltage to drop below the specified threshold.
 * Used to verify that the camera ECU has been powered off (e.g., < 1.0V).
 * Polls every 1 second until voltage drops below threshold or timeout.
 *
 * Keywords: voltage below, power off check, low voltage, ECU off verification,
 *           AIN threshold, voltage drop, power cut check
 * Parameters:
 *   threshold  - voltage threshold in Volts (e.g., 1.0)
 *   timeout_ms - maximum wait time in milliseconds
 * Return: 1 = voltage below threshold (PASS), 0 = timeout (still above)
 */
int Check_AIN_Below_Threshold(float threshold, long timeout_ms)
{
  long elapsed = 0;
  long log_interval;
  float ain_voltage = 0.0;

  write("Checking AIN voltage below %.1f V... (max %d seconds)", threshold,
        timeout_ms / 1000);

  log_interval = (timeout_ms / 6 / 1000) * 1000;
  if (log_interval < 1000)
    log_interval = 1000;

  while (elapsed < timeout_ms)
  {
    ain_voltage = Get_AIN_Voltage();
    if (ain_voltage < threshold)
    {
      write(">>> PASS: AIN = %.2f V < %.1f V <<<", ain_voltage, threshold);
      return 1;
    }
    if (elapsed == 0 || elapsed % log_interval == 0)
      write("  [%d s] AIN=%.2f V (waiting for < %.1f V)", elapsed / 1000,
            ain_voltage, threshold);

    testWaitForTimeout(1000);
    elapsed += 1000;
  }
  write(">>> TIMEOUT: AIN = %.2f V still >= %.1f V <<<", ain_voltage,
        threshold);
  return 0;
}

/*
 * Check_AIN_Above_Threshold(float threshold, long timeout_ms)
 * ------------------------------------------------------------
 * Waits for AIN voltage to rise above the specified threshold.
 * Used to verify that the camera ECU has been powered on (e.g., > 11.0V).
 * Polls every 1 second until voltage rises above threshold or timeout.
 *
 * Keywords: voltage above, power on check, high voltage, ECU on verification,
 *           AIN threshold, voltage rise, power supply check
 * Parameters:
 *   threshold  - voltage threshold in Volts (e.g., 11.0)
 *   timeout_ms - maximum wait time in milliseconds
 * Return: 1 = voltage above threshold (PASS), 0 = timeout (still below)
 */
int Check_AIN_Above_Threshold(float threshold, long timeout_ms)
{
  long elapsed = 0;
  long log_interval;
  float ain_voltage = 0.0;

  write("Checking AIN voltage above %.1f V... (max %d seconds)", threshold,
        timeout_ms / 1000);

  log_interval = (timeout_ms / 6 / 1000) * 1000;
  if (log_interval < 1000)
    log_interval = 1000;

  while (elapsed < timeout_ms)
  {
    ain_voltage = Get_AIN_Voltage();
    if (ain_voltage > threshold)
    {
      write(">>> PASS: AIN = %.2f V > %.1f V <<<", ain_voltage, threshold);
      return 1;
    }
    if (elapsed == 0 || elapsed % log_interval == 0)
      write("  [%d s] AIN=%.2f V (waiting for > %.1f V)", elapsed / 1000,
            ain_voltage, threshold);

    testWaitForTimeout(1000);
    elapsed += 1000;
  }
  write(">>> TIMEOUT: AIN = %.2f V still <= %.1f V <<<", ain_voltage,
        threshold);
  return 0;
}

// =============================================================================
// SECTION 10: SCREENSHOT / CAPTURE (CANoe Test Report)
// =============================================================================
//
// Purpose: Capture screenshots of a specified CANoe panel window for test
// reports.
//          Adds the captured image with signal annotations to the test report.
// Keywords: screenshot, capture, test report, panel capture, window capture,
//           testReportAddWindowCapture, evidence, visual verification
// =============================================================================

/*
 * CaptureGraphics(char window[], char signalList[])
 * --------------------------------------------------
 * Captures a screenshot of the specified panel window and adds it to the
 * CANoe test report with the specified signal list annotation.
 * Waits 3 seconds before capture to ensure the panel is fully rendered.
 *
 * Keywords: screenshot, capture panel, test report image, window capture,
 *           visual evidence, testReportAddWindowCapture, signal annotation
 * Parameters:
 *   window[]     - name of the CANoe panel window to capture (e.g., "OWP",
 * "OWD") signalList[] - comma-separated string of signal names to annotate in
 * the report (e.g., "BLTN_CAM_RecSta_OWP,BLTN_CAM_RecSet_OWP") Return: void
 */
void CaptureGraphics(char window[], char signalList[])
{
  testWaitForTimeout(3000);
  testReportAddWindowCapture(window, signalList, "Screenshot");
  write(">>> Screenshot captured <<<");
}

/*
 * OTA_Reset_AppStateWaitContext()
 * --------------------------------
 * Resets shared wait context before starting a new OTA flow.
 * Must be called right before launching the python update command.
 */
void OTA_Reset_AppStateWaitContext()
{
  gOTA_AppStateWaitStart = @PythonCom::AppState;
  gOTA_AppStateTrigger = 0;
}

/*
 * OTA_Update_AppStateTrigger_FromValue(int previousState, int currentState)
 * --------------------------------------------------------------------------
 * Latches AppState when it changes to 3 or 4.
 * Call this from local `on sysvar_update PythonCom::AppState` handlers.
 */
void OTA_Update_AppStateTrigger_FromValue(int previousState, int currentState)
{
  if (currentState == 3 && previousState != 3)
  {
    gOTA_AppStateTrigger = 3;
  }
  else if (currentState == 4 && previousState != 4)
  {
    gOTA_AppStateTrigger = 4;
  }
}

/*
 * Wait_AppState_3_Or_4_Changed(long timeoutMs)
 * ---------------------------------------------
 * Waits until AppState changes to 3 or 4.
 * Effective timeout is capped at 30 minutes (1800000 ms).
 * Uses both latched trigger (from sysvar update handler) and direct read
 * fallback, so short AppState pulses are captured more reliably.
 *
 * Return:
 *   3 = AppState changed to 3
 *   4 = AppState changed to 4
 *   0 = timeout
 */
int Wait_AppState_3_Or_4_Changed(long timeoutMs)
{
  long elapsed;
  long effectiveTimeoutMs;

  elapsed = 0;
  effectiveTimeoutMs = timeoutMs;
  if (effectiveTimeoutMs <= 0 || effectiveTimeoutMs > 1800000)
  {
    effectiveTimeoutMs = 1800000;
  }

  while (elapsed < effectiveTimeoutMs)
  {
    if (gOTA_AppStateTrigger == 3 || gOTA_AppStateTrigger == 4)
    {
      return gOTA_AppStateTrigger;
    }

    if (@PythonCom::AppState != gOTA_AppStateWaitStart)
    {
      if (@PythonCom::AppState == 3 || @PythonCom::AppState == 4)
      {
        return @PythonCom::AppState;
      }
    }

    if (elapsed > 0 && elapsed % 60000 == 0)
    {
      write("[OTA_WAIT] Waiting AppState... elapsed=%d s, current=%d",
            elapsed / 1000,
            @PythonCom::AppState);
    }

    testWaitForTimeout(1000);
    elapsed += 1000;
  }

  return 0;
}

// =============================================================================
// SECTION 11: COUNTDOWN TIMER (Wait with Visual Countdown)
// =============================================================================
//
// Purpose: Wait for a specified duration while displaying a countdown in the
//          write window. Useful for visualizing long wait periods in test
//          execution (e.g., waiting for parking time to expire).
// Keywords: countdown, timer, wait, delay, clock, remaining time, display,
//           visual countdown, progress indicator, MM:SS format
// =============================================================================

/*
 * Clock_Down_ms(long CD_time)
 * ---------------------------
 * Countdown timer with millisecond input. Displays remaining time in MM:SS
 * format every second. Total wait = CD_time milliseconds.
 *
 * Keywords: countdown milliseconds, timer ms, wait with countdown,
 *           remaining time display, second-by-second countdown
 * Parameter: CD_time - total countdown time in milliseconds (e.g., 60000 for 1
 * min) Return: void (blocks until countdown complete)
 */
void Clock_Down_ms(long CD_time)
{
  write("=== COUNT DOWN: %d seconds ===", CD_time / 1000);
  while (CD_time > 0)
  {
    write("  Remaining: %02d:%02d", CD_time / (1000 * 60),
          (CD_time / 1000) % 60);
    testWaitForTimeout(1000);
    CD_time -= 1000;
  }
  write("=== COUNT DOWN COMPLETE ===");
}

// =============================================================================
// SECTION 13: DRIVING INFO SIMULATION (Vehicle Speed / GPS Simulation)
// =============================================================================
//
// Purpose: Enable or disable driving information simulation via panel sysvar.
//          When enabled, the system simulates vehicle driving data (speed, GPS,
//          etc.) which triggers the camera ECU to enter "driving" mode.
//          Required for testing OWD (driving recording) and DEV (driving
//          event).
// Keywords: driving info, driving simulation, vehicle speed, GPS simulation,
//           EnableDrvInfo, panel sysvar, driving mode, vehicle motion,
//           simulate driving, simulate parking, driving state
// =============================================================================

/*
 * Enable_DrivingInfo()
 * --------------------
 * Enables driving info simulation (vehicle appears to be driving).
 * Camera ECU will treat this as vehicle-in-motion state.
 *
 * Keywords: enable driving, start driving simulation, vehicle moving,
 *           driving mode on, EnableDrvInfo on
 * System variable: @sysvar::panel::EnableDrvInfo = 1
 * Wait: 3000ms for state to take effect
 * Return: void
 */
void Enable_DrivingInfo()
{
  write("=== Enabling Driving Info Simulation ===");
  @sysvar::panel::EnableDrvInfo = 1;
  testWaitForTimeout(3000);
  write("  EnableDrvInfo = ON");
}

/*
 * Disable_DrivingInfo()
 * ---------------------
 * Disables driving info simulation (vehicle appears to be parked/stationary).
 * Camera ECU will treat this as vehicle-parked state.
 *
 * Keywords: disable driving, stop driving simulation, vehicle parked,
 *           driving mode off, EnableDrvInfo off, simulate parked
 * System variable: @sysvar::panel::EnableDrvInfo = 0
 * Wait: 3000ms for state to take effect
 * Return: void
 */
void Disable_DrivingInfo()
{
  write("=== Disabling Driving Info Simulation ===");
  @sysvar::panel::EnableDrvInfo = 0;
  testWaitForTimeout(3000);
  write("  EnableDrvInfo = OFF");
}

// =============================================================================
// SECTION 14: COMMON INIT SEQUENCE (Standard Test Setup / Teardown)
// =============================================================================
//
// Purpose: Provide standard initialization and cleanup procedures that most
//          test cases share. Reduces boilerplate code in individual test files.
// Keywords: initialization, init, setup, cleanup, teardown, test preparation,
//           standard init, test start, test end, CGW NM, SMK, USM Reset,
//           Rec Reset, boilerplate, common setup
//
// Standard_Init flow:
//   1. Enable CGW Network Management (wake up CAN bus)
//   2. Set SMK to IGN (ignition on)
//   3. Click USM Reset (reset storage)
//   4. Click Rec Reset (format SD card)
// =============================================================================

/*
 * Standard_Init()
 * ---------------
 * Performs standard test initialization sequence:
 *   1. @panel::Switch_CGWNM = 1 (enable CGW NM for CAN communication)
 *   2. Set_SMK_IGN() (set ignition to ON)
 *   3. Click_USM_Reset() (reset update storage memory)
 *   4. Click_Rec_Reset() (format SD card / reset recordings)
 *
 * Call this at the beginning of each test case for a clean starting state.
 *
 * Keywords: standard init, test initialization, setup procedure, test start,
 *           CGW NM enable, ignition on, USM reset, SD format, clean state
 * Return: void
 */
void Standard_Init()
{
  write("=== Standard Test Initialization ===");
  Set_SMK_IGN();
  Click_USM_Reset();
  Click_Rec_Reset();
  write("=== Initialization Complete ===");
}

// =============================================================================
// SECTION 14B: ERROR DETAIL LOGGING
// =============================================================================
//
// Purpose: Provide a comprehensive snapshot of all key CAN signal values
//          when a test step fails. Call this function inside every fail branch
//          to aid in root cause analysis.
// Keywords: error logging, debug, diagnostic, signal dump, fail detail,
//           error snapshot, troubleshooting, all signals
// =============================================================================

/*
 * Log_Error_Details()
 * --------------------
 * Logs the current value of all key CAN signals used in OWP testing.
 * Call this function in every testStepFail branch to capture a full
 * system snapshot at the moment of failure.
 *
 * Signals logged:
 *   - SMK terminal status (ignition / power state)
 *   - BLTN_CAM UI mode, overall camera state
 *   - ICU power auto-cut mode
 *   - All RecSet signals (OWD, OWP, DEV, PEV)
 *   - All RecSta signals (OWD, OWP, DEV, PEV, MAR, TML, PMD)
 *
 * Keywords: error detail, signal snapshot, fail log, all signals dump
 * Return: void
 */
void Log_Error_Details()
{
  char buf[200];
  write("");
  write("================================================================");
  write("=          [ERROR DETAILS] Signal Snapshot at Failure          =");
  write("================================================================");

  snprintf(buf, elcount(buf),
           "SMK_TrmnlCtrlGrpStaBDC=0x%X, HU_UI_Mode=0x%X, State=0x%X, "
           "ICU_Power=0x%X",
           $SMK_TrmnlCtrlGrpStaBDC, $BLTN_CAM_HU_UI_Mode, $BLTN_CAM_State,
           $ICU_PowerAutoCutModSta);
  testStep("DIAG_SYS", buf);
  write("  SMK_TrmnlCtrlGrpStaBDC  = 0x%X (0=OFF, 1=ACC, 2=IGN)",
        $SMK_TrmnlCtrlGrpStaBDC);
  write("  BLTN_CAM_HU_UI_Mode     = 0x%X (0=AVN, 1=BLTN_CAM)",
        $BLTN_CAM_HU_UI_Mode);
  write("  BLTN_CAM_State          = 0x%X", $BLTN_CAM_State);
  write("  ICU_PowerAutoCutModSta  = 0x%X (0=Factory, 1=Dealer, 2=Customer)",
        $ICU_PowerAutoCutModSta);

  write("  ---- Recording Settings (RecSet) ----");
  snprintf(buf, elcount(buf), "RecSet: OWD=0x%X, OWP=0x%X, DEV=0x%X, PEV=0x%X",
           $BLTN_CAM_RecSet_OWD, $BLTN_CAM_RecSet_OWP, $BLTN_CAM_RecSet_DEV,
           $BLTN_CAM_RecSet_PEV);
  testStep("DIAG_REC_SET", buf);
  write("  RecSet_OWD = 0x%X   RecSet_OWP = 0x%X   (0x1=OFF, 0x2=ON)",
        $BLTN_CAM_RecSet_OWD, $BLTN_CAM_RecSet_OWP);
  write("  RecSet_DEV = 0x%X   RecSet_PEV = 0x%X   (0x1=OFF, 0x2=ON)",
        $BLTN_CAM_RecSet_DEV, $BLTN_CAM_RecSet_PEV);

  write("  ---- Recording Status (RecSta) ----");
  snprintf(buf, elcount(buf),
           "RecSta: OWD=0x%X, OWP=0x%X, DEV=0x%X, PEV=0x%X, MAR=0x%X, "
           "TML=0x%X, PMD=0x%X",
           $BLTN_CAM_RecSta_OWD, $BLTN_CAM_RecSta_OWP, $BLTN_CAM_RecSta_DEV,
           $BLTN_CAM_RecSta_PEV, $BLTN_CAM_RecSta_MAR, $BLTN_CAM_RecSta_TML,
           $BLTN_CAM_RecSta_PMD);
  testStep("DIAG_REC_STA", buf);
  write("  RecSta_OWD = 0x%X   RecSta_OWP = 0x%X   (0x0=OFF, 0x1=Recording)",
        $BLTN_CAM_RecSta_OWD, $BLTN_CAM_RecSta_OWP);
  write("  RecSta_DEV = 0x%X   RecSta_PEV = 0x%X", $BLTN_CAM_RecSta_DEV,
        $BLTN_CAM_RecSta_PEV);
  write("  RecSta_MAR = 0x%X   RecSta_TML = 0x%X", $BLTN_CAM_RecSta_MAR,
        $BLTN_CAM_RecSta_TML);
  write("  RecSta_PMD = 0x%X", $BLTN_CAM_RecSta_PMD);

  write("================================================================");
  write("");
}

// =============================================================================
// SECTION 15: BLTN_CAM SETTINGS (Panel TrackBar Controls)
// =============================================================================
//
// Purpose: Configure camera recording parameters through CANoe panel trackbar
//          (slider) controls. Each setting has a Check function (instant
//          verify) and a Set function (set value + internal check + retry up to
//          15 times).
// Keywords: trackbar, slider, panel settings, camera configuration,
//           sensitivity, camera angle, parking time, event time, timelapse,
//           DEV sensitivity, PEV sensitivity, PMD range, camera vertical
//           position, front camera, rear camera, OWP parking time, event before
//           time, event after time, TML interval, Set with check, retry logic
//
// Pattern for all Set_* functions in this section:
//   1. Validate input range
//   2. Set the trackbar value via @sysvar::panel::TrackBar_XXX
//   3. Wait 3000ms for CAN feedback
//   4. Call Check_XXX() to verify
//   5. If check fails, retry up to 15 times (re-set trackbar each time)
//   6. Final confirmation check, return 1 (PASS) or 0 (FAIL)
//
// All Check_* functions: read CAN feedback signal, compare to expected, return
// 1/0 All Set_* functions: set + check + retry, return 1 (success) or 0 (failed
// after retries)
// =============================================================================

// ---------------------------------------------------------------------------
// DEV Sensitivity (Driving Event Sensitivity)
// ---------------------------------------------------------------------------
// Controls sensitivity level for driving event (impact) detection.
// Higher level = more sensitive (triggers on smaller impacts).
//
// TrackBar: @sysvar::panel::TrackBar_DEV_Sen
// Range: 1~5 (Lv.1=least sensitive ~ Lv.5=most sensitive)
// Feedback signal: BLTN_CAM_SenSet_DEV (0x1~0x5, direct mapping)
// Keywords: DEV sensitivity, driving event sensitivity, impact sensitivity,
//           G-sensor sensitivity, collision detection level,
//           BLTN_CAM_SenSet_DEV
// ---------------------------------------------------------------------------

/*
 * Check_DEV_Sensitivity(int expected_level)
 * Instantly checks if DEV sensitivity matches expected level (1~5).
 * Signal: BLTN_CAM_SenSet_DEV
 * Return: 1 = PASS, 0 = FAIL
 */
int Check_DEV_Sensitivity(int expected_level)
{
  byte val;
  val = $BLTN_CAM_SenSet_DEV;
  write("  BLTN_CAM_SenSet_DEV = 0x%X", val);
  if (val == expected_level)
  {
    write(">>> PASS: DEV Sensitivity = Lv.%d <<<", expected_level);
    return 1;
  }
  else
  {
    write(">>> FAIL: DEV Sensitivity = 0x%X, expected Lv.%d <<<", val,
          expected_level);
    return 0;
  }
}

/*
 * Set_DEV_Sensitivity(int level)
 * Sets DEV sensitivity to specified level (1~5) with internal check and retry.
 * TrackBar: @sysvar::panel::TrackBar_DEV_Sen
 * Signal: BLTN_CAM_SenSet_DEV
 * Retry: up to 15 attempts
 * Return: 1 = success, 0 = failed after retries
 */
int Set_DEV_Sensitivity(int level)
{
  int retry = 0;
  write("=== Setting DEV Sensitivity to Lv.%d ===", level);
  if (level < 1 || level > 5)
  {
    write(">>> ERROR: Invalid level %d (must be 1~5) <<<", level);
    return 0;
  }

  if (Check_DEV_Sensitivity(level) == 1)
  {
    write(">>> SET OK: DEV Sensitivity already Lv.%d, no action required <<<",
          level);
    return 1;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  @sysvar::panel::TrackBar_DEV_Sen = level;
  testWaitForTimeout(3000);

  while (Check_DEV_Sensitivity(level) == 0 && retry < 15)
  {
    write("  Retry %d/15: Re-setting DEV Sensitivity...", retry + 1);
    @sysvar::panel::TrackBar_DEV_Sen = level;
    testWaitForTimeout(3000);
    retry++;
  }

  if ($BLTN_CAM_SenSet_DEV == level)
  {
    Exit_BLTN_CAM();
    testWaitForTimeout(1000);
    return 1;
  }
  else
  {
    write(">>> FAILED: Could not set DEV Sensitivity after %d retries <<<",
          retry);
    Exit_BLTN_CAM();
    testWaitForTimeout(1000);
    return 0;
  }
}

// ---------------------------------------------------------------------------
// PEV Sensitivity (Parking Event Sensitivity)
// ---------------------------------------------------------------------------
// Controls sensitivity level for parking event (impact) detection.
// Higher level = more sensitive (triggers on smaller impacts while parked).
//
// TrackBar: @sysvar::panel::TrackBar_PEV_Sen
// Range: 1~5 (Lv.1=least sensitive ~ Lv.5=most sensitive)
// Feedback signal: BLTN_CAM_SenSet_PEV (0x1~0x5, direct mapping)
// Keywords: PEV sensitivity, parking event sensitivity, parking impact,
//           parking G-sensor, parking collision level, BLTN_CAM_SenSet_PEV
// ---------------------------------------------------------------------------

/*
 * Check_PEV_Sensitivity(int expected_level)
 * Instantly checks if PEV sensitivity matches expected level (1~5).
 * Signal: BLTN_CAM_SenSet_PEV
 * Return: 1 = PASS, 0 = FAIL
 */
int Check_PEV_Sensitivity(int expected_level)
{
  byte val;
  val = $BLTN_CAM_SenSet_PEV;
  write("  BLTN_CAM_SenSet_PEV = 0x%X", val);
  if (val == expected_level)
  {
    write(">>> PASS: PEV Sensitivity = Lv.%d <<<", expected_level);
    return 1;
  }
  else
  {
    write(">>> FAIL: PEV Sensitivity = 0x%X, expected Lv.%d <<<", val,
          expected_level);
    return 0;
  }
}

/*
 * Set_PEV_Sensitivity(int level)
 * Sets PEV sensitivity to specified level (1~5) with internal check and retry.
 * TrackBar: @sysvar::panel::TrackBar_PEV_Sen
 * Signal: BLTN_CAM_SenSet_PEV
 * Retry: up to 15 attempts
 * Return: 1 = success, 0 = failed after retries
 */
int Set_PEV_Sensitivity(int level)
{
  int retry = 0;
  write("=== Setting PEV Sensitivity to Lv.%d ===", level);
  if (level < 1 || level > 5)
  {
    write(">>> ERROR: Invalid level %d (must be 1~5) <<<", level);
    return 0;
  }

  if (Check_PEV_Sensitivity(level) == 1)
  {
    write(">>> SET OK: PEV Sensitivity already Lv.%d, no action required <<<",
          level);
    return 1;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  @sysvar::panel::TrackBar_PEV_Sen = level;
  testWaitForTimeout(3000);

  while (Check_PEV_Sensitivity(level) == 0 && retry < 15)
  {
    write("  Retry %d/15: Re-setting PEV Sensitivity...", retry + 1);
    @sysvar::panel::TrackBar_PEV_Sen = level;
    testWaitForTimeout(3000);
    retry++;
  }

  if ($BLTN_CAM_SenSet_PEV == level)
  {
    Exit_BLTN_CAM();
    testWaitForTimeout(1000);
    return 1;
  }
  else
  {
    write(">>> FAILED: Could not set PEV Sensitivity after %d retries <<<",
          retry);
    Exit_BLTN_CAM();
    testWaitForTimeout(1000);
    return 0;
  }
}

// ---------------------------------------------------------------------------
// PMD Range (Parking Motion Detection Range)
// ---------------------------------------------------------------------------
// Controls the detection range for parking motion detection.
// Higher level = wider detection range.
//
// TrackBar: @sysvar::panel::TrackBar_PMD_RanSet
// Range: 1~5 (Lv.1=shortest range ~ Lv.5=widest range)
// Feedback signal: BLTN_CAM_RanSet_PMDR (0x1~0x5, direct mapping)
// Keywords: PMD range, motion detection range, parking motion range,
//           detection distance, sensor range, BLTN_CAM_RanSet_PMDR
// ---------------------------------------------------------------------------

/*
 * Check_PMD_RanSet(int expected_level)
 * Instantly checks if PMD detection range matches expected level (1~5).
 * Signal: BLTN_CAM_RanSet_PMDR
 * Return: 1 = PASS, 0 = FAIL
 */
int Check_PMD_RanSet(int expected_level)
{
  byte val;
  val = $BLTN_CAM_RanSet_PMDR;
  write("  BLTN_CAM_RanSet_PMDR = 0x%X", val);
  if (val == expected_level)
  {
    write(">>> PASS: PMD_RanSet = Lv.%d <<<", expected_level);
    return 1;
  }
  else
  {
    write(">>> FAIL: PMD_RanSet = 0x%X, expected Lv.%d <<<", val,
          expected_level);
    return 0;
  }
}

/*
 * Set_PMD_RanSet(int level)
 * Sets PMD detection range to specified level (1~5) with internal check and
 * retry. Signal: BLTN_CAM_Ran_PMDR_NvalueSet (on HU_BLTN_CAM_01_00ms) Retry: up
 * to 15 attempts Return: 1 = success, 0 = failed after retries
 */
int Set_PMD_RanSet(int level)
{
  int retry = 0;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;
  write("=== Setting PMD Range to Lv.%d ===", level);
  if (level < 1 || level > 5)
  {
    write(">>> ERROR: Invalid level %d (must be 1~5) <<<", level);
    return 0;
  }

  if (Check_PMD_RanSet(level) == 1)
  {
    write(">>> SET OK: PMD Range already Lv.%d, no action required <<<", level);
    return 1;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  _HU_BLTN_CAM_01_00ms.BLTN_CAM_Ran_PMDR_NvalueSet = level;
  output(_HU_BLTN_CAM_01_00ms);
  testWaitForTimeout(3000);

  while (Check_PMD_RanSet(level) == 0 && retry < 15)
  {
    write("  Retry %d/15: Re-setting PMD Range...", retry + 1);
    _HU_BLTN_CAM_01_00ms.BLTN_CAM_Ran_PMDR_NvalueSet = level;
    output(_HU_BLTN_CAM_01_00ms);
    testWaitForTimeout(3000);
    retry++;
  }

  if ($BLTN_CAM_RanSet_PMDR == level)
  {
    Exit_BLTN_CAM();
    testWaitForTimeout(1000);
    return 1;
  }
  else
  {
    write(">>> FAILED: Could not set PMD Range after %d retries <<<", retry);
    Exit_BLTN_CAM();
    testWaitForTimeout(1000);
    return 0;
  }
}

// ---------------------------------------------------------------------------
// Front Camera Vertical Position (FR Camera Angle Adjustment)
// ---------------------------------------------------------------------------
// Adjusts the vertical viewing angle of the front camera.
// TrackBar range 1~11 maps to angle offset -5 to +5 degrees.
//
// TrackBar: @sysvar::panel::TrackBar_FR_Ver_Pos
// Range: 1~11
//   Trackbar  1  2  3  4  5  6  7  8  9 10 11
//   Angle    -5 -4 -3 -2 -1  0 +1 +2 +3 +4 +5
//   CAN    0x15 0x14 0x13 0x12 0x11 0x0 0x1 0x2 0x3 0x4 0x5
//
// Feedback signal: BLTN_CAM_Set_FR_Ver_Pos (CAN enum value)
// Keywords: front camera, vertical position, camera angle, FR camera,
//           tilt angle, viewing angle, BLTN_CAM_Set_FR_Ver_Pos, FR_Ver_Pos
//
// Note: Uses FR_RR_Trackbar_To_CAN() helper for trackbar-to-CAN conversion.
//       Same conversion formula shared with Rear Camera (RR).
// ---------------------------------------------------------------------------

/*
 * FR_RR_Trackbar_To_CAN(int trackbar_val)
 * ----------------------------------------
 * Helper function: converts trackbar value (1~11) to CAN enum value.
 * Used by both Front Camera and Rear Camera position functions.
 *
 * Mapping logic:
 *   trackbar 1~5  -> negative angles: CAN = 0x16 - trackbar (0x15..0x11)
 *   trackbar 6    -> center (0 degrees): CAN = 0x0
 *   trackbar 7~11 -> positive angles: CAN = trackbar - 6 (0x1..0x5)
 *
 * Keywords: trackbar to CAN, angle conversion, camera position mapping,
 *           enum conversion, FR RR helper, vertical position mapping
 * Parameter: trackbar_val - trackbar position (1~11)
 * Return: CAN enum value, or -1 if invalid input
 */
int FR_RR_Trackbar_To_CAN(int trackbar_val)
{
  // 1->0x15(-5), 2->0x14(-4), ..., 5->0x11(-1), 6->0x0(0), 7->0x1(+1), ...,
  // 11->0x5(+5)
  if (trackbar_val >= 1 && trackbar_val <= 5)
    return 0x16 - trackbar_val;
  else if (trackbar_val == 6)
    return 0x0;
  else if (trackbar_val >= 7 && trackbar_val <= 11)
    return trackbar_val - 6;
  return -1;
}

/*
 * Check_FR_Cam_VerPos(int expected_trackbar_val)
 * Checks if front camera vertical position matches expected trackbar value.
 * Converts trackbar to CAN value for comparison.
 * Signal: BLTN_CAM_Set_FR_Ver_Pos
 * Return: 1 = PASS, 0 = FAIL
 */
int Check_FR_Cam_VerPos(int expected_trackbar_val)
{
  int expected_can;
  byte val;
  expected_can = FR_RR_Trackbar_To_CAN(expected_trackbar_val);
  val = $BLTN_CAM_Set_FR_Ver_Pos;
  write("  BLTN_CAM_Set_FR_Ver_Pos = 0x%X (expected CAN=0x%X, trackbar=%d)",
        val, expected_can, expected_trackbar_val);
  if (val == expected_can)
  {
    write(">>> PASS: FR Camera VerPos matched <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: FR Camera VerPos mismatch <<<");
    return 0;
  }
}

/*
 * Set_FR_Cam_VerPos(int trackbar_val)
 * Sets front camera vertical position with internal check and retry.
 * TrackBar: @sysvar::panel::TrackBar_FR_Ver_Pos
 * Signal: BLTN_CAM_Set_FR_Ver_Pos
 * Retry: up to 15 attempts
 * Return: 1 = success, 0 = failed after retries
 */
int Set_FR_Cam_VerPos(int trackbar_val)
{
  int retry = 0;
  int result;
  write("=== Setting Front Camera Vertical Position = %d ===", trackbar_val);
  if (trackbar_val < 1 || trackbar_val > 11)
  {
    write(">>> ERROR: Invalid value %d (must be 1~11) <<<", trackbar_val);
    return 0;
  }

  if (Check_FR_Cam_VerPos(trackbar_val) == 1)
  {
    write(">>> SET OK: Front Camera VerPos already %d, no action required <<<",
          trackbar_val);
    return 1;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(1000);

  @sysvar::panel::TrackBar_FR_Ver_Pos = trackbar_val;
  testWaitForTimeout(1000);

  while (Check_FR_Cam_VerPos(trackbar_val) == 0 && retry < 5)
  {
    write("  Retry %d/5: Re-setting FR Camera VerPos...", retry + 1);
    @sysvar::panel::TrackBar_FR_Ver_Pos = trackbar_val;
    testWaitForTimeout(1000);
    retry++;
  }

  if ($BLTN_CAM_Set_FR_Ver_Pos == FR_RR_Trackbar_To_CAN(trackbar_val))
  {
    result = 1;
  }
  else
  {
    write(">>> FAILED: Could not set FR Camera VerPos after %d retries <<<",
          retry);
    result = 0;
  }

  Exit_BLTN_CAM();
  // testWaitForTimeout(1000);
  return result;
}

// ---------------------------------------------------------------------------
// Rear Camera Vertical Position (RR Camera Angle Adjustment)
// ---------------------------------------------------------------------------
// Adjusts the vertical viewing angle of the rear camera.
// Same trackbar-to-CAN mapping as Front Camera (uses FR_RR_Trackbar_To_CAN).
//
// TrackBar: @sysvar::panel::TrackBar_RR_Ver_Pos
// Range: 1~11 (same mapping as Front Camera above)
// Feedback signal: BLTN_CAM_Set_RR_Ver_Pos (CAN enum value)
// Keywords: rear camera, vertical position, camera angle, RR camera,
//           tilt angle, back camera, BLTN_CAM_Set_RR_Ver_Pos, RR_Ver_Pos
// ---------------------------------------------------------------------------

/*
 * Check_RR_Cam_VerPos(int expected_trackbar_val)
 * Checks if rear camera vertical position matches expected trackbar value.
 * Signal: BLTN_CAM_Set_RR_Ver_Pos
 * Return: 1 = PASS, 0 = FAIL
 */
int Check_RR_Cam_VerPos(int expected_trackbar_val)
{
  int expected_can;
  byte val;
  expected_can = FR_RR_Trackbar_To_CAN(expected_trackbar_val);
  val = $BLTN_CAM_Set_RR_Ver_Pos;
  write("  BLTN_CAM_Set_RR_Ver_Pos = 0x%X (expected CAN=0x%X, trackbar=%d)",
        val, expected_can, expected_trackbar_val);
  if (val == expected_can)
  {
    write(">>> PASS: RR Camera VerPos matched <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RR Camera VerPos mismatch <<<");
    return 0;
  }
}

/*
 * Set_RR_Cam_VerPos(int trackbar_val)
 * Sets rear camera vertical position with internal check and retry.
 * TrackBar: @sysvar::panel::TrackBar_RR_Ver_Pos
 * Signal: BLTN_CAM_Set_RR_Ver_Pos
 * Retry: up to 15 attempts
 * Return: 1 = success, 0 = failed after retries
 */
int Set_RR_Cam_VerPos(int trackbar_val)
{
  int retry = 0;
  int result;
  write("=== Setting Rear Camera Vertical Position = %d ===", trackbar_val);
  if (trackbar_val < 1 || trackbar_val > 11)
  {
    write(">>> ERROR: Invalid value %d (must be 1~11) <<<", trackbar_val);
    return 0;
  }

  if (Check_RR_Cam_VerPos(trackbar_val) == 1)
  {
    write(">>> SET OK: Rear Camera VerPos already %d, no action required <<<",
          trackbar_val);
    return 1;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(1000);

  @sysvar::panel::TrackBar_RR_Ver_Pos = trackbar_val;
  testWaitForTimeout(1000);

  while (Check_RR_Cam_VerPos(trackbar_val) == 0 && retry < 5)
  {
    write("  Retry %d/5: Re-setting RR Camera VerPos...", retry + 1);
    @sysvar::panel::TrackBar_RR_Ver_Pos = trackbar_val;
    testWaitForTimeout(1000);
    retry++;
  }

  if ($BLTN_CAM_Set_RR_Ver_Pos == FR_RR_Trackbar_To_CAN(trackbar_val))
  {
    result = 1;
  }
  else
  {
    write(">>> FAILED: Could not set RR Camera VerPos after %d retries <<<",
          retry);
    result = 0;
  }

  Exit_BLTN_CAM();
  // testWaitForTimeout(1000);
  return result;
}

// ---------------------------------------------------------------------------
// OWP Parking Time (Continuous Parking Recording Duration)
// ---------------------------------------------------------------------------
// Sets how long the camera continues recording after the vehicle is parked
// (key OFF). After this time expires, OWP recording stops to save battery.
//
// TrackBar: @sysvar::panel::TrackBar_OWP_TIME
// Range: 1~20 (hours) - trackbar value = hours directly
// Feedback signal: BLTN_CAM_Set_PrkTime (0x1~0x14 = 1Hr~20Hr, direct mapping)
// Keywords: OWP parking time, parking duration, recording time, hours,
//           PrkTime, parking recording duration, battery time,
//           BLTN_CAM_Set_PrkTime, OWP time setting, how long to record
// ---------------------------------------------------------------------------

/*
 * Check_OWP_ParkingTime(int expected_hours)
 * Checks if OWP parking time matches expected hours (1~20).
 * Signal: BLTN_CAM_Set_PrkTime
 * Return: 1 = PASS, 0 = FAIL
 */
int Check_OWP_ParkingTime(int expected_hours)
{
  byte val;
  val = $BLTN_CAM_Set_PrkTime;
  write("  BLTN_CAM_Set_PrkTime = 0x%X", val);
  if (val == expected_hours)
  {
    write(">>> PASS: PrkTime = 0x%X (%dHr) <<<", val, expected_hours);
    return 1;
  }
  else
  {
    write(">>> FAIL: PrkTime = 0x%X, expected 0x%X (%dHr) <<<", val,
          expected_hours, expected_hours);
    return 0;
  }
}

/*
 * Check_OWP_ParkingTime_InRange()
 * Checks if OWP parking time is in valid range 0x1~0x14 (1Hr~20Hr).
 * Does NOT check specific value, only validates range.
 * Signal: BLTN_CAM_Set_PrkTime
 * Keywords: parking time range check, valid range, PrkTime in range
 * Return: 1 = in range (PASS), 0 = out of range (FAIL)
 */
int Check_OWP_ParkingTime_InRange()
{
  byte val;
  val = $BLTN_CAM_Set_PrkTime;
  write("  BLTN_CAM_Set_PrkTime = 0x%X", val);
  if (val >= 0x1 && val <= 0x14)
  {
    write(">>> PASS: PrkTime = 0x%X in range 1~20Hr <<<", val);
    return 1;
  }
  else
  {
    write(">>> FAIL: PrkTime = 0x%X out of range <<<", val);
    return 0;
  }
}

/*
 * Set_OWP_ParkingTime(int hours)
 * Sets OWP parking time to specified hours (1~20) with internal check and
 * retry. TrackBar: @sysvar::panel::TrackBar_OWP_TIME Signal:
 * BLTN_CAM_Set_PrkTime Retry: up to 15 attempts Return: 1 = success, 0 = failed
 * after retries
 */
int Set_OWP_ParkingTime(int hours)
{
  int retry = 0;
  write("=== Setting OWP Parking Time = %d Hr ===", hours);
  if (hours < 1 || hours > 20)
  {
    write(">>> ERROR: Invalid value %d (must be 1~20) <<<", hours);
    return 0;
  }

  if (Check_OWP_ParkingTime(hours) == 1)
  {
    write(">>> SET OK: OWP Parking Time already %d Hr, no action required <<<",
          hours);
    return 1;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  @sysvar::panel::TrackBar_OWP_TIME = hours;
  testWaitForTimeout(3000);

  while (Check_OWP_ParkingTime(hours) == 0 && retry < 15)
  {
    write("  Retry %d/15: Re-setting OWP Parking Time...", retry + 1);
    @sysvar::panel::TrackBar_OWP_TIME = hours;
    testWaitForTimeout(3000);
    retry++;
  }

  if ($BLTN_CAM_Set_PrkTime == hours)
  {
    Exit_BLTN_CAM();
    testWaitForTimeout(1000);
    return 1;
  }
  else
  {
    write(">>> FAILED: Could not set OWP Parking Time after %d retries <<<",
          retry);
    Exit_BLTN_CAM();
    testWaitForTimeout(1000);
    return 0;
  }
}

// ---------------------------------------------------------------------------
// Event Before Time (Pre-event Buffer Duration)
// ---------------------------------------------------------------------------
// Controls how many seconds of video BEFORE the event are saved.
// When an event (impact) is detected, the camera saves a clip that includes
// footage from X seconds before the event occurred.
//
// TrackBar: @sysvar::panel::TrackBar_EV_Time_Bfr
// Range: 1~3 (INVERSE mapping - higher trackbar = shorter time!)
//   Trackbar 1 -> 20 seconds before event (CAN BfrTime_NvalueSet = 0xA)
//   Trackbar 2 -> 15 seconds before event (CAN BfrTime_NvalueSet = 0x5)
//   Trackbar 3 -> 10 seconds before event (CAN BfrTime_NvalueSet = 0x0)
//
// Feedback signal: BLTN_CAM_Set_EV_BfrTime
//   Actual seconds = BLTN_CAM_Set_EV_BfrTime + 10
//   (raw 0xA -> 10+10=20s, raw 0x5 -> 5+10=15s, raw 0x0 -> 0+10=10s)
//
// Keywords: event before time, pre-event buffer, before event seconds,
//           BfrTime, pre-recording, pre-event video, event clip before,
//           BLTN_CAM_Set_EV_BfrTime, EV_Time_Bfr
// ---------------------------------------------------------------------------

/*
 * Check_EV_BeforeTime(int expected_trackbar)
 * Checks if event before-time matches the expected trackbar setting.
 * Converts trackbar (1=20s, 2=15s, 3=10s) to seconds for comparison.
 * Signal: BLTN_CAM_Set_EV_BfrTime (actual = raw + 10)
 * Return: 1 = PASS, 0 = FAIL
 */
int Check_EV_BeforeTime(int expected_trackbar)
{
  int expected_seconds;
  int actual_seconds;

  if (expected_trackbar == 1)
    expected_seconds = 20;
  else if (expected_trackbar == 2)
    expected_seconds = 15;
  else
    expected_seconds = 10;

  actual_seconds = $BLTN_CAM_Set_EV_BfrTime + 10;
  write("  EV BfrTime = %ds (expected %ds)", actual_seconds, expected_seconds);

  if (actual_seconds == expected_seconds)
  {
    write(">>> PASS: EV BfrTime = %ds <<<", actual_seconds);
    return 1;
  }
  else
  {
    write(">>> FAIL: EV BfrTime = %ds, expected %ds <<<", actual_seconds,
          expected_seconds);
    return 0;
  }
}

/*
 * Set_EV_BeforeTime(int trackbar_val)
 * Sets event before-time via trackbar (1=20s, 2=15s, 3=10s) with retry.
 * TrackBar: @sysvar::panel::TrackBar_EV_Time_Bfr
 * Signal: BLTN_CAM_Set_EV_BfrTime
 * Retry: up to 15 attempts
 * Return: 1 = success, 0 = failed after retries
 */
int Set_EV_BeforeTime(int trackbar_val)
{
  int retry = 0;
  write("=== Setting Event Before Time (Trackbar=%d) ===", trackbar_val);
  if (trackbar_val < 1 || trackbar_val > 3)
  {
    write(">>> ERROR: Invalid value %d (must be 1~3) <<<", trackbar_val);
    return 0;
  }

  if (Check_EV_BeforeTime(trackbar_val) == 1)
  {
    write(">>> SET OK: Event Before Time already matches, no action required <<<");
    return 1;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  @sysvar::panel::TrackBar_EV_Time_Bfr = trackbar_val;
  testWaitForTimeout(3000);

  while (Check_EV_BeforeTime(trackbar_val) == 0 && retry < 15)
  {
    write("  Retry %d/15: Re-setting EV Before Time...", retry + 1);
    @sysvar::panel::TrackBar_EV_Time_Bfr = trackbar_val;
    testWaitForTimeout(3000);
    retry++;
  }

  {
    int expected_seconds;
    int actual_seconds;
    if (trackbar_val == 1)
      expected_seconds = 20;
    else if (trackbar_val == 2)
      expected_seconds = 15;
    else
      expected_seconds = 10;
    actual_seconds = $BLTN_CAM_Set_EV_BfrTime + 10;
    if (actual_seconds == expected_seconds)
    {
      Exit_BLTN_CAM();
      testWaitForTimeout(1000);
      return 1;
    }
    else
    {
      write(">>> FAILED: Could not set EV Before Time after %d retries <<<",
            retry);
      Exit_BLTN_CAM();
      testWaitForTimeout(1000);
      return 0;
    }
  }
}

// ---------------------------------------------------------------------------
// Event After Time (Post-event Buffer Duration)
// ---------------------------------------------------------------------------
// Controls how many seconds of video AFTER the event are saved.
// When an event (impact) is detected, the camera continues recording for
// X more seconds after the event.
//
// TrackBar: @sysvar::panel::TrackBar_EV_Time_Aft
// Values: 10, 15, or 20 (seconds - direct input, NOT trackbar index!)
//   10 seconds -> CAN AftTime_NvalueSet = 0x0
//   15 seconds -> CAN AftTime_NvalueSet = 0x5
//   20 seconds -> CAN AftTime_NvalueSet = 0xA
//
// Feedback signal: BLTN_CAM_Set_EV_AftTime
//   Actual seconds = BLTN_CAM_Set_EV_AftTime + 10
//   (raw 0x0 -> 0+10=10s, raw 0x5 -> 5+10=15s, raw 0xA -> 10+10=20s)
//
// Keywords: event after time, post-event buffer, after event seconds,
//           AftTime, post-recording, post-event video, event clip after,
//           BLTN_CAM_Set_EV_AftTime, EV_Time_Aft
// ---------------------------------------------------------------------------

/*
 * Check_EV_AfterTime(int expected_seconds)
 * Checks if event after-time matches expected seconds (10, 15, or 20).
 * Signal: BLTN_CAM_Set_EV_AftTime (actual = raw + 10)
 * Return: 1 = PASS, 0 = FAIL
 */
int Check_EV_AfterTime(int expected_seconds)
{
  int actual_seconds;
  actual_seconds = $BLTN_CAM_Set_EV_AftTime + 10;
  write("  EV AftTime = %ds (expected %ds)", actual_seconds, expected_seconds);

  if (actual_seconds == expected_seconds)
  {
    write(">>> PASS: EV AftTime = %ds <<<", actual_seconds);
    return 1;
  }
  else
  {
    write(">>> FAIL: EV AftTime = %ds, expected %ds <<<", actual_seconds,
          expected_seconds);
    return 0;
  }
}

/*
 * Set_EV_AfterTime(int seconds)
 * Sets event after-time in seconds (10, 15, or 20) with retry.
 * TrackBar: @sysvar::panel::TrackBar_EV_Time_Aft
 * Signal: BLTN_CAM_Set_EV_AftTime
 * Retry: up to 15 attempts
 * Return: 1 = success, 0 = failed after retries
 */
int Set_EV_AfterTime(int seconds)
{
  int retry = 0;
  write("=== Setting Event After Time = %ds ===", seconds);
  if (seconds != 10 && seconds != 15 && seconds != 20)
  {
    write(">>> ERROR: Invalid value %d (must be 10, 15, or 20) <<<", seconds);
    return 0;
  }

  if (Check_EV_AfterTime(seconds) == 1)
  {
    write(">>> SET OK: Event After Time already %ds, no action required <<<",
          seconds);
    return 1;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  @sysvar::panel::TrackBar_EV_Time_Aft = seconds;
  testWaitForTimeout(3000);

  while (Check_EV_AfterTime(seconds) == 0 && retry < 15)
  {
    write("  Retry %d/15: Re-setting EV After Time...", retry + 1);
    @sysvar::panel::TrackBar_EV_Time_Aft = seconds;
    testWaitForTimeout(3000);
    retry++;
  }

  {
    int actual_seconds;
    actual_seconds = $BLTN_CAM_Set_EV_AftTime + 10;
    if (actual_seconds == seconds)
    {
      Exit_BLTN_CAM();
      testWaitForTimeout(1000);
      return 1;
    }
    else
    {
      write(">>> FAILED: Could not set EV After Time after %d retries <<<",
            retry);
      Exit_BLTN_CAM();
      testWaitForTimeout(1000);
      return 0;
    }
  }
}

// ---------------------------------------------------------------------------
// Time-lapse Interval (TML Recording Interval)
// ---------------------------------------------------------------------------
// Controls the interval between time-lapse photo captures.
// Time-lapse mode takes periodic snapshots to create a sped-up video.
//
// TrackBar: @sysvar::panel::TrackBar_Timelaps_TIME
// Range: 1~3 (NON-LINEAR mapping!)
//   Trackbar 1 -> 20 minutes  (CAN TML_Int = 0x1)
//   Trackbar 2 -> 40 minutes  (CAN TML_Int = 0x2)
//   Trackbar 3 -> 100 minutes (CAN TML_Int = 0x5)
//
// Feedback signal: BLTN_CAM_Set_Drv_TML_Int (CAN enum value)
// Keywords: time-lapse interval, TML interval, timelapse time, capture
// interval,
//           snapshot interval, BLTN_CAM_Set_Drv_TML_Int, Timelaps_TIME,
//           20min 40min 100min
// ---------------------------------------------------------------------------

/*
 * Check_TML_Interval(int expected_trackbar)
 * Checks if TML interval matches expected trackbar (1=20min, 2=40min,
 * 3=100min). Converts trackbar to CAN value for comparison. Signal:
 * BLTN_CAM_Set_Drv_TML_Int Return: 1 = PASS, 0 = FAIL
 */
int Check_TML_Interval(int expected_trackbar)
{
  int expected_can;
  byte val;

  if (expected_trackbar == 1)
    expected_can = 0x1;
  else if (expected_trackbar == 2)
    expected_can = 0x2;
  else
    expected_can = 0x5;

  val = $BLTN_CAM_Set_Drv_TML_Int;
  write("  BLTN_CAM_Set_Drv_TML_Int = 0x%X (expected 0x%X)", val, expected_can);

  if (val == expected_can)
  {
    write(">>> PASS: TML Interval matched <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: TML Interval mismatch <<<");
    return 0;
  }
}

/*
 * Set_TML_Interval(int trackbar_val)
 * Sets TML interval via trackbar (1=20min, 2=40min, 3=100min) with retry.
 * TrackBar: @sysvar::panel::TrackBar_Timelaps_TIME
 * Signal: BLTN_CAM_Set_Drv_TML_Int
 * Retry: up to 15 attempts
 * Return: 1 = success, 0 = failed after retries
 */
int Set_TML_Interval(int trackbar_val)
{
  int retry = 0;
  int expected_can;
  char desc[20];

  if (trackbar_val == 1)
  {
    snprintf(desc, 20, "%s", "20min");
    expected_can = 0x1;
  }
  else if (trackbar_val == 2)
  {
    snprintf(desc, 20, "%s", "40min");
    expected_can = 0x2;
  }
  else if (trackbar_val == 3)
  {
    snprintf(desc, 20, "%s", "100min");
    expected_can = 0x5;
  }
  else
  {
    write(">>> ERROR: Invalid value %d (must be 1, 2, or 3) <<<", trackbar_val);
    return 0;
  }

  write("=== Setting Time-lapse Interval = %s (Trackbar=%d) ===", desc,
        trackbar_val);

  if (Check_TML_Interval(trackbar_val) == 1)
  {
    write(">>> SET OK: Time-lapse Interval already %s, no action required <<<",
          desc);
    return 1;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  @sysvar::panel::TrackBar_Timelaps_TIME = trackbar_val;
  testWaitForTimeout(3000);

  while (Check_TML_Interval(trackbar_val) == 0 && retry < 15)
  {
    write("  Retry %d/15: Re-setting TML Interval...", retry + 1);
    @sysvar::panel::TrackBar_Timelaps_TIME = trackbar_val;
    testWaitForTimeout(3000);
    retry++;
  }

  if ($BLTN_CAM_Set_Drv_TML_Int == expected_can)
  {
    Exit_BLTN_CAM();
    testWaitForTimeout(1000);
    return 1;
  }
  else
  {
    write(">>> FAILED: Could not set TML Interval after %d retries <<<", retry);
    Exit_BLTN_CAM();
    testWaitForTimeout(1000);
    return 0;
  }
}

// ---------------------------------------------------------------------------
// PEV App Notification (Parking Event App Notification)
// ---------------------------------------------------------------------------
// Controls whether a notification is sent to the app when a parking event
// occurs.
//
// TrackBar/Panel: none currently integrated
// Feedback signal: BLTN_CAM_Set_PEV_AppNoti (0x1=Off, 0x2=On)
// Set signal: BLTN_CAM_AppNoti_PEV_NvalueSet
// Keywords: PEV app notification, app notification,
// BLTN_CAM_AppNoti_PEV_NvalueSet
// ---------------------------------------------------------------------------

/*
 * Check_PEV_AppNoti(int expected_value)
 * Checks if PEV App Notification matches expected value.
 * Signal: BLTN_CAM_Set_PEV_AppNoti
 * Return: 1 = PASS, 0 = FAIL
 */
int Check_PEV_AppNoti(int expected_value)
{
  byte val;
  val = $BLTN_CAM_Set_PEV_AppNoti;
  write("  BLTN_CAM_Set_PEV_AppNoti = 0x%X", val);
  if (val == expected_value)
  {
    write(">>> PASS: PEV App Notification = 0x%X <<<", expected_value);
    return 1;
  }
  else
  {
    write(">>> FAIL: PEV App Notification = 0x%X, expected 0x%X <<<", val,
          expected_value);
    return 0;
  }
}

/*
 * Set_PEV_AppNoti(int value)
 * Sets PEV App Notification to specified value with internal check and retry.
 * Signal: BLTN_CAM_AppNoti_PEV_NvalueSet (on HU_BLTN_CAM_01_00ms)
 * Retry: up to 10 attempts
 * Return: 1 = success, 0 = failed after retries
 */
int Set_PEV_AppNoti(int value)
{
  int retry = 0;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;
  write("=== Setting PEV App Notification to %d ===", value);
  if (value < 0 || value > 3)
  {
    write(">>> ERROR: Invalid value %d (must be 0~3) <<<", value);
    return 0;
  }

  if (Check_PEV_AppNoti(value) == 1)
  {
    write(">>> SET OK: PEV App Notification already 0x%X, no action required <<<",
          value);
    return 1;
  }

  _HU_BLTN_CAM_01_00ms.BLTN_CAM_AppNoti_PEV_NvalueSet = value;
  output(_HU_BLTN_CAM_01_00ms);
  testWaitForTimeout(3000);

  while (Check_PEV_AppNoti(value) == 0 && retry < 10)
  {
    write("  Retry %d/10: Re-setting PEV App Notification...", retry + 1);
    _HU_BLTN_CAM_01_00ms.BLTN_CAM_AppNoti_PEV_NvalueSet = value;
    output(_HU_BLTN_CAM_01_00ms);
    testWaitForTimeout(3000);
    retry++;
  }

  if ($BLTN_CAM_Set_PEV_AppNoti == value)
  {
    return 1;
  }
  else
  {
    write(">>> FAILED: Could not set PEV App Notification after %d retries <<<",
          retry);
    return 0;
  }
}

// ---------------------------------------------------------------------------
// WaitFor_RecSta_PEV_OFF(long timeout_s)
// ---------------------------------------------------------------------------
// Polls BLTN_CAM_RecSta_PEV every 1 second until it equals 0x0 (OFF).
// Used after PEV recording starts to detect when PEV recording stops.
// This allows measuring the PEV recording duration.
//
// Signal: BLTN_CAM_RecSta_PEV (0x0 = OFF/Stopped, 0x1 = ON/Recording)
// Parameter: timeout_s = max wait time in seconds
// Return: 1 = RecSta_PEV became 0x0 (OFF), 0 = timeout
//
// Example:
//   // PEV already recording
//   if (WaitFor_RecSta_PEV_OFF(30) == 1) {
//       write("PEV recording stopped");
//   }
// ---------------------------------------------------------------------------
int WaitFor_RecSta_PEV_OFF(long timeout_s)
{
  long i;
  write("  Waiting for BLTN_CAM_RecSta_PEV = 0x0 (OFF) (max %ds)...",
        timeout_s);
  for (i = 0; i < timeout_s; i++)
  {
    if ($BLTN_CAM_RecSta_PEV == 0x0)
    {
      write(">>> [OK] BLTN_CAM_RecSta_PEV = 0x0 (OFF) after %ds <<<", i);
      return 1;
    }
    testWaitForTimeout(1000);
  }
  write(">>> [NG] BLTN_CAM_RecSta_PEV = 0x%X after %ds <<<",
        $BLTN_CAM_RecSta_PEV, timeout_s);
  return 0;
}

// ---------------------------------------------------------------------------
// WaitFor_RecSta_OWD_OFF(int timeout_s)
// ---------------------------------------------------------------------------
// Polls BLTN_CAM_RecSta_OWD every 1 second until it equals 0x0 (OFF).
// Used after OWD recording starts to detect when OWD recording stops.
//
// Signal: BLTN_CAM_RecSta_OWD (0x0 = OFF/Stopped, 0x1 = ON/Recording)
// Parameter: timeout_s = max wait time in seconds
// Return: 1 = RecSta_OWD became 0x0 (OFF), 0 = timeout
// ---------------------------------------------------------------------------
int WaitFor_RecSta_OWD_OFF(int timeout_s)
{
  int i;
  write("  Waiting for BLTN_CAM_RecSta_OWD = 0x0 (OFF) (max %ds)...",
        timeout_s);
  for (i = 0; i < timeout_s; i++)
  {
    if ($BLTN_CAM_RecSta_OWD == 0x0)
    {
      write(">>> [OK] BLTN_CAM_RecSta_OWD = 0x0 (OFF) after %ds <<<", i);
      return 1;
    }
    testWaitForTimeout(1000);
  }
  write(">>> [NG] BLTN_CAM_RecSta_OWD = 0x%X after %ds <<<",
        $BLTN_CAM_RecSta_OWD, timeout_s);
  return 0;
}

// ---------------------------------------------------------------------------
// WaitFor_RecSta_OWP_OFF(int timeout_s)
// ---------------------------------------------------------------------------
// Polls BLTN_CAM_RecSta_OWP every 1 second until it equals 0x0 (OFF).
// Used after OWP recording starts to detect when OWP recording stops.
//
// Signal: BLTN_CAM_RecSta_OWP (0x0 = OFF/Stopped, 0x1 = ON/Recording)
// Parameter: timeout_s = max wait time in seconds
// Return: 1 = RecSta_OWP became 0x0 (OFF), 0 = timeout
// ---------------------------------------------------------------------------
int WaitFor_RecSta_OWP_OFF(int timeout_s)
{
  int i;
  write("  Waiting for BLTN_CAM_RecSta_OWP = 0x0 (OFF) (max %ds)...",
        timeout_s);
  for (i = 0; i < timeout_s; i++)
  {
    if ($BLTN_CAM_RecSta_OWP == 0x0)
    {
      write(">>> [OK] BLTN_CAM_RecSta_OWP = 0x0 (OFF) after %ds <<<", i);
      return 1;
    }
    testWaitForTimeout(1000);
  }
  write(">>> [NG] BLTN_CAM_RecSta_OWP = 0x%X after %ds <<<",
        $BLTN_CAM_RecSta_OWP, timeout_s);
  return 0;
}

// ---------------------------------------------------------------------------
// WaitFor_RecSta_DEV_OFF(long timeout_s)
// ---------------------------------------------------------------------------
// Polls BLTN_CAM_RecSta_DEV every 1 second until it equals 0x0 (OFF).
// Used after DEV recording starts to detect when DEV recording stops.
//
// Signal: BLTN_CAM_RecSta_DEV (0x0 = OFF/Stopped, 0x1 = ON/Recording)
// Parameter: timeout_s = max wait time in seconds
// Return: 1 = RecSta_DEV became 0x0 (OFF), 0 = timeout
// ---------------------------------------------------------------------------
int WaitFor_RecSta_DEV_OFF(long timeout_s)
{
  long i;
  write("  Waiting for BLTN_CAM_RecSta_DEV = 0x0 (OFF) (max %ds)...",
        timeout_s);
  for (i = 0; i < timeout_s; i++)
  {
    if ($BLTN_CAM_RecSta_DEV == 0x0)
    {
      write(">>> [OK] BLTN_CAM_RecSta_DEV = 0x0 (OFF) after %ds <<<", i);
      return 1;
    }
    testWaitForTimeout(1000);
  }
  write(">>> [NG] BLTN_CAM_RecSta_DEV = 0x%X after %ds <<<",
        $BLTN_CAM_RecSta_DEV, timeout_s);
  return 0;
}

// ---------------------------------------------------------------------------
// WaitFor_RecSta_TML_OFF(int timeout_s)
// ---------------------------------------------------------------------------
// Polls BLTN_CAM_RecSta_TML every 1 second until it equals 0x0 (OFF).
// Used after TML recording starts to detect when TML recording stops.
//
// Signal: BLTN_CAM_RecSta_TML (0x0 = OFF/Stopped, 0x1 = ON/Recording)
// Parameter: timeout_s = max wait time in seconds
// Return: 1 = RecSta_TML became 0x0 (OFF), 0 = timeout
// ---------------------------------------------------------------------------
int WaitFor_RecSta_TML_OFF(int timeout_s)
{
  int i;
  write("  Waiting for BLTN_CAM_RecSta_TML = 0x0 (OFF) (max %ds)...",
        timeout_s);
  for (i = 0; i < timeout_s; i++)
  {
    if ($BLTN_CAM_RecSta_TML == 0x0)
    {
      write(">>> [OK] BLTN_CAM_RecSta_TML = 0x0 (OFF) after %ds <<<", i);
      return 1;
    }
    testWaitForTimeout(1000);
  }
  write(">>> [NG] BLTN_CAM_RecSta_TML = 0x%X after %ds <<<",
        $BLTN_CAM_RecSta_TML, timeout_s);
  return 0;
}

// ---------------------------------------------------------------------------
// WaitFor_RecSta_MAR_OFF(int timeout_s)
// ---------------------------------------------------------------------------
// Polls BLTN_CAM_RecSta_MAR every 1 second until it equals 0x0 (OFF).
// Used after MAR recording starts to detect when MAR recording stops.
//
// Signal: BLTN_CAM_RecSta_MAR (0x0 = OFF/Stopped, 0x1 = ON/Recording)
// Parameter: timeout_s = max wait time in seconds
// Return: 1 = RecSta_MAR became 0x0 (OFF), 0 = timeout
// ---------------------------------------------------------------------------
int WaitFor_RecSta_MAR_OFF(int timeout_s)
{
  int i;
  write("  Waiting for BLTN_CAM_RecSta_MAR = 0x0 (OFF) (max %ds)...",
        timeout_s);
  for (i = 0; i < timeout_s; i++)
  {
    if ($BLTN_CAM_RecSta_MAR == 0x0)
    {
      write(">>> [OK] BLTN_CAM_RecSta_MAR = 0x0 (OFF) after %ds <<<", i);
      return 1;
    }
    testWaitForTimeout(1000);
  }
  write(">>> [NG] BLTN_CAM_RecSta_MAR = 0x%X after %ds <<<",
        $BLTN_CAM_RecSta_MAR, timeout_s);
  return 0;
}

// ===========================================================================
// SECTION 17: HU CLOCK CONTROL
// ===========================================================================
//
// The BLTN_CAM simulation node sends HU_CLOCK_01_1000ms every second while
// panel::Switch_Clock is ON. These helpers also output the CAN FD message
// directly so automated tests do not depend only on the panel event path.
// Both functions are idempotent and retry confirmation at most five times.
//
// Return: 1 = requested state confirmed, 0 = failed after five attempts
// ===========================================================================

int Turn_ON_HU_Clock()
{
  message HU_CLOCK_01_1000ms hu_clock_message;
  long local_time[9];
  int retry = 0;

  if (@sysvar::panel::Switch_Clock == 1 &&
      getSignal(HU_Status) == 0x2 &&
      getSignal(HU_TimeSetting) == 0x2)
  {
    write(">>> SET OK: HU clock already ON, no action required <<<");
    return 1;
  }

  while (retry < 5)
  {
    retry++;
    getLocalTime(local_time);

    hu_clock_message.HU_DisplayYear1 = 0x1;
    hu_clock_message.HU_DisplayYear2 = (local_time[5] + 1900) - 2000;
    hu_clock_message.HU_DisplayMonth = local_time[4] + 1;
    hu_clock_message.HU_DisplayDay = local_time[3];
    hu_clock_message.HU_DisplayHour = local_time[2];
    hu_clock_message.HU_DisplayMinute = local_time[1];
    hu_clock_message.HU_DisplaySecond = local_time[0];
    hu_clock_message.HU_Status = 0x2;
    hu_clock_message.HU_TimeSetting = 0x2;

    @sysvar::panel::Switch_Clock = 1;
    output(hu_clock_message);
    write("[HU CLOCK] ON attempt %d/5: sent HU_CLOCK_01_1000ms", retry);
    testWaitForTimeout(1200);

    if (@sysvar::panel::Switch_Clock == 1 &&
        getSignal(HU_Status) == 0x2 &&
        getSignal(HU_TimeSetting) == 0x2)
    {
      write(">>> [OK] HU clock ON confirmed after attempt %d <<<", retry);
      return 1;
    }

    write("[HU CLOCK] ON not confirmed: Switch=%d, Status=0x%X, TimeSetting=0x%X",
          @sysvar::panel::Switch_Clock, getSignal(HU_Status),
          getSignal(HU_TimeSetting));
  }

  write(">>> [NG] Failed to turn HU clock ON after 5 attempts <<<");
  return 0;
}

int Turn_OFF_HU_Clock()
{
  message HU_CLOCK_01_1000ms hu_clock_message;
  int retry = 0;

  if (@sysvar::panel::Switch_Clock == 0 &&
      getSignal(HU_Status) == 0x0 &&
      getSignal(HU_TimeSetting) == 0x0)
  {
    write(">>> SET OK: HU clock already OFF, no action required <<<");
    return 1;
  }

  while (retry < 5)
  {
    retry++;
    @sysvar::panel::Switch_Clock = 0;
    hu_clock_message.HU_Status = 0x0;
    hu_clock_message.HU_TimeSetting = 0x0;
    output(hu_clock_message);
    write("[HU CLOCK] OFF attempt %d/5: sent HU_CLOCK_01_1000ms", retry);
    testWaitForTimeout(1200);

    if (@sysvar::panel::Switch_Clock == 0 &&
        getSignal(HU_Status) == 0x0 &&
        getSignal(HU_TimeSetting) == 0x0)
    {
      write(">>> [OK] HU clock OFF confirmed after attempt %d <<<", retry);
      return 1;
    }

    write("[HU CLOCK] OFF not confirmed: Switch=%d, Status=0x%X, TimeSetting=0x%X",
          @sysvar::panel::Switch_Clock, getSignal(HU_Status),
          getSignal(HU_TimeSetting));
  }

  write(">>> [NG] Failed to turn HU clock OFF after 5 attempts <<<");
  return 0;
}

// ===========================================================================
// SECTION 19: BURGLAR ALARM SIGNAL
// ===========================================================================
//
// Purpose: Set BAlarm_BglrAlrm_CurrentSta signal via CAN FD message
//          BDC_FD_04_200ms (ID 0x3D3) using Rising Edge technique:
//          first reset to 0x0, then set to target value.
//
// Signal: BAlarm_BglrAlrm_CurrentSta (4-bit, bit 24, message BDC_FD_04_200ms)
// Valid values (0x0 ~ 0x9):
//   0x0 = Off
//   0x1 = DisArm
//   0x2 = Arm
//   0x3 = PreArm
//   0x4 = ArmWait
//   0x5 = ArmHold
//   0x6 = ReArm
//   0x7 = AutoLockTimer (AutoLockTimer1)
//   0x8 = RESERVED
//   0x9 = Alarm
//
// Functions:
//   Check_BglrAlrm(int expected_value)  - Check current signal value
//   Set_BglrAlrm(int value)             - Set with Rising Edge + verify
// ===========================================================================

/*
 * Check_BglrAlrm(int expected_value)
 * Reads BAlarm_BglrAlrm_CurrentSta and compares against expected_value.
 * Signal: BAlarm_BglrAlrm_CurrentSta
 * Return: 1 = match, 0 = mismatch
 */
int Check_BglrAlrm(int expected_value)
{
  int val;
  val = $BAlarm_BglrAlrm_CurrentSta;
  write("  BAlarm_BglrAlrm_CurrentSta = 0x%X", val);
  if (val == expected_value)
  {
    write(">>> [OK] BAlarm_BglrAlrm_CurrentSta = 0x%X (match) <<<",
          expected_value);
    return 1;
  }
  else
  {
    write(">>> [NG] BAlarm_BglrAlrm_CurrentSta = 0x%X, expected 0x%X <<<", val,
          expected_value);
    return 0;
  }
}

/*
 * Set_BglrAlrm(int value)
 * Sets Burglar Alarm signal via IL (Interaction Layer) using Rising Edge
 * technique:
 *   1. Set 0x0 (Low) via IL to reset the signal first
 *   2. Set target value (Rising Edge) via IL
 *   3. Verify signal with retry (max 10 times, 500ms interval)
 * Uses $signal syntax to update IL's internal value so CGW_CCU cyclic sends
 * maintain the value (avoids overwrite by IL's 200ms cyclic message).
 * Signal: BAlarm_BglrAlrm_CurrentSta on BDC_FD_04_200ms (ID 0x3D3)
 * Valid range: 0x0 ~ 0x9
 * Return: 1 = success (signal confirmed), 0 = failed after retries
 */
int Set_BglrAlrm(int value)
{
  int retry;
  retry = 0;

  write("=== Set_BglrAlrm: Rising Edge 0x0 -> 0x%X ===", value);

  if (value < 0x0 || value > 0x9)
  {
    write(">>> ERROR: Invalid value 0x%X (valid range: 0x0 ~ 0x9) <<<", value);
    return 0;
  }

  if (Check_BglrAlrm(value) == 1)
  {
    write(">>> SET OK: BglrAlrm already 0x%X, no action required <<<", value);
    return 1;
  }

  // Step 1: Rising Edge - reset to 0x0 (Low) first
  write("  [1/3] Reset to 0x0 (Low)...");
  $BAlarm_BglrAlrm_CurrentSta = 0x0;
  testWaitForTimeout(2000);
  write("  BAlarm_BglrAlrm_CurrentSta = 0x%X", $BAlarm_BglrAlrm_CurrentSta);

  // Step 2: Rising Edge - set to target value (High) via IL
  write("  [2/3] Set to 0x%X (Rising Edge)...", value);
  $BAlarm_BglrAlrm_CurrentSta = value;

  // Step 3: Verify with retry
  while ($BAlarm_BglrAlrm_CurrentSta != value && retry < 10)
  {
    write("  Retry %d/10: Re-sending 0x%X...", retry + 1, value);
    $BAlarm_BglrAlrm_CurrentSta = value;
    testWaitForTimeout(500);
    retry++;
  }

  if ($BAlarm_BglrAlrm_CurrentSta == value)
  {
    write(">>> [OK] Set_BglrAlrm = 0x%X confirmed <<<", value);
    return 1;
  }
  else
  {
    write(">>> [FAIL] Set_BglrAlrm: still 0x%X after %d retries <<<",
          $BAlarm_BglrAlrm_CurrentSta, retry);
    return 0;
  }
}

// =============================================================================
// SECTION 20: PMD CONTROL (Parking Motion Detection)
// =============================================================================
//
// Purpose: Control and verify PMD-related settings on the Gen 2.5 Rader panel.
//          PMD has 3 sub-toggles and 1 range setting:
//            PMD     = Parking Motion Detection main toggle
//            PMDEV   = PMD Event recording toggle
//            PMDTime = PMD Time-based recording toggle
//            PMDR    = PMD Radar detection range (handled in Section 15)
//
// Signal naming (setting status, response from camera ECU):
//   BLTN_CAM_Set_PMD      : 0x1 = OFF, 0x2 = ON
//   BLTN_CAM_Set_PMDEV    : 0x1 = OFF, 0x2 = ON
//   BLTN_CAM_Set_PMDTime  : 0x1 = OFF, 0x2 = ON
//
// Panel variables (Rader panel):
//   @panel_Rader::LED_Ctrl_Set_PMD
//   @panel_Rader::LED_Ctrl_Set_PMDEV
//   @panel_Rader::LED_Ctrl_Set_PMDTime
//
// Recording status:
//   BLTN_CAM_RecSta_PMD   : 0x0 = OFF, 0x1 = Recording
// =============================================================================

// ---------------------------------------------------------------------------
// PMD Setting Check (Instant Verification)
// ---------------------------------------------------------------------------

/*
 * Check_Set_PMD_ON() / Check_Set_PMD_OFF()
 * Check if PMD (Parking Motion Detection) setting is ON (0x2) or OFF (0x1).
 * Signal: BLTN_CAM_Set_PMD
 * Keywords: PMD setting, parking motion detection setting
 */
int Check_Set_PMD_ON()
{
  byte val;
  val = $BLTN_CAM_Set_PMD;
  write("  BLTN_CAM_Set_PMD = 0x%X", val);
  if (val == 0x2)
  {
    write(">>> PASS: Set_PMD = ON <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: Set_PMD is NOT ON <<<");
    return 0;
  }
}

int Check_Set_PMD_OFF()
{
  byte val;
  val = $BLTN_CAM_Set_PMD;
  write("  BLTN_CAM_Set_PMD = 0x%X", val);
  if (val == 0x1)
  {
    write(">>> PASS: Set_PMD = OFF <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: Set_PMD is NOT OFF <<<");
    return 0;
  }
}

/*
 * Check_Set_PMDEV_ON() / Check_Set_PMDEV_OFF()
 * Check if PMDEV (PMD Event) setting is ON (0x2) or OFF (0x1).
 * Signal: BLTN_CAM_Set_PMDEV
 * Keywords: PMDEV setting, PMD event setting
 */
int Check_Set_PMDEV_ON()
{
  byte val;
  val = $BLTN_CAM_Set_PMDEV;
  write("  BLTN_CAM_Set_PMDEV = 0x%X", val);
  if (val == 0x2)
  {
    write(">>> PASS: Set_PMDEV = ON <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: Set_PMDEV is NOT ON <<<");
    return 0;
  }
}

int Check_Set_PMDEV_OFF()
{
  byte val;
  val = $BLTN_CAM_Set_PMDEV;
  write("  BLTN_CAM_Set_PMDEV = 0x%X", val);
  if (val == 0x1)
  {
    write(">>> PASS: Set_PMDEV = OFF <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: Set_PMDEV is NOT OFF <<<");
    return 0;
  }
}

/*
 * Check_Set_PMDTime_ON() / Check_Set_PMDTime_OFF()
 * Check if PMDTime setting is ON (0x2) or OFF (0x1).
 * Signal: BLTN_CAM_Set_PMDTime
 * Keywords: PMDTime setting, PMD time setting
 */
int Check_Set_PMDTime_ON()
{
  byte val;
  val = $BLTN_CAM_Set_PMDTime;
  write("  BLTN_CAM_Set_PMDTime = 0x%X", val);
  if (val == 0x2)
  {
    write(">>> PASS: Set_PMDTime = ON <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: Set_PMDTime is NOT ON <<<");
    return 0;
  }
}

int Check_Set_PMDTime_OFF()
{
  byte val;
  val = $BLTN_CAM_Set_PMDTime;
  write("  BLTN_CAM_Set_PMDTime = 0x%X", val);
  if (val == 0x1)
  {
    write(">>> PASS: Set_PMDTime = OFF <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: Set_PMDTime is NOT OFF <<<");
    return 0;
  }
}

// ---------------------------------------------------------------------------
// PMD RecSta Check (Recording Active Status)
// ---------------------------------------------------------------------------

/*
 * Check_RecSta_PMD_Recording() / Check_RecSta_PMD_OFF()
 * Check if PMD is actively recording (0x1) or stopped (0x0).
 * Signal: BLTN_CAM_RecSta_PMD
 * Keywords: PMD status, parking motion detection recording active, PMD RecSta
 */
int Check_RecSta_PMD_Recording()
{
  byte val;
  val = $BLTN_CAM_RecSta_PMD;
  write("  BLTN_CAM_RecSta_PMD = 0x%X", val);
  if (val == 0x1)
  {
    write(">>> PASS: RecSta_PMD = Recording <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSta_PMD is NOT Recording <<<");
    return 0;
  }
}

int Check_RecSta_PMD_OFF()
{
  byte val;
  val = $BLTN_CAM_RecSta_PMD;
  write("  BLTN_CAM_RecSta_PMD = 0x%X", val);
  if (val == 0x0)
  {
    write(">>> PASS: RecSta_PMD = OFF <<<");
    return 1;
  }
  else
  {
    write(">>> FAIL: RecSta_PMD is NOT OFF <<<");
    return 0;
  }
}

// ---------------------------------------------------------------------------
// PMD Toggle (Turn ON / Turn OFF via Rader Panel)
// ---------------------------------------------------------------------------

/*
 * Turn_ON_PMD()
 * Enables Parking Motion Detection (PMD) via Rader panel LED toggle.
 * Retries up to 10 times until BLTN_CAM_Set_PMD == 0x2 (ON).
 *
 * Keywords: PMD on, enable parking motion detection, LED toggle,
 *           BLTN_CAM_Set_PMD, panel_Rader LED_Ctrl_Set_PMD
 * Panel variable: @panel_Rader::LED_Ctrl_Set_PMD = 1
 * Signal checked: BLTN_CAM_Set_PMD == 0x2 (ON)
 * Retry: up to 10 attempts, 2000ms between each
 * Return: void (logs SUCCESS/FAILED)
 */
void Turn_ON_PMD()
{
  int retry = 0;
  int is_on = 0;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;

  write("=== Turn ON PMD via panel ===");

  is_on = Check_Set_PMD_ON();
  if (is_on == 1)
  {
    write(">>> SET OK: PMD already ON (0x2), no action required <<<");
    return;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  is_on = Check_Set_PMD_ON();
  while (is_on == 0 && retry < 10)
  {
    write("  Attempt %d: Force PMD OFF/default edge, then PMD ON...", retry + 1);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_PMD_NvalueSet = 0x1;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_PMD_NvalueSet = 0x1");
    testWaitForTimeout(500);

    @panel_Rader::LED_Ctrl_Set_PMD = 0;
    testWaitForTimeout(100);
    @panel_Rader::LED_Ctrl_Set_PMD = 1;
    testWaitForTimeout(200);
    @panel_Rader::LED_Ctrl_Set_PMD = 0;
    testWaitForTimeout(500);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_PMD_NvalueSet = 0x2;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_PMD_NvalueSet = 0x2");

    testWaitForTimeout(3000);
    is_on = Check_Set_PMD_ON();
    retry++;
  }

  if (is_on == 1)
    write(">>> SUCCESS: PMD = ON (0x2) <<<");
  else
    write(">>> FAILED: Could not set PMD to ON <<<");

  Exit_BLTN_CAM();
  testWaitForTimeout(1000);
}

/*
 * Turn_OFF_PMD()
 * Disables Parking Motion Detection (PMD) via Rader panel LED toggle.
 * Retries up to 15 times while BLTN_CAM_Set_PMD is still 0x2 (ON).
 *
 * Keywords: PMD off, disable parking motion detection, LED toggle,
 *           BLTN_CAM_Set_PMD, panel_Rader LED_Ctrl_Set_PMD
 * Panel variable: @panel_Rader::LED_Ctrl_Set_PMD = 1
 * Signal checked: BLTN_CAM_Set_PMD == 0x1 (OFF)
 * Retry: up to 15 attempts, 2000ms between each
 * Return: void (logs SUCCESS/FAILED)
 */
void Turn_OFF_PMD()
{
  int retry = 0;
  int is_off = 0;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;

  write("=== Turn OFF PMD via panel ===");

  is_off = Check_Set_PMD_OFF();
  if (is_off == 1)
  {
    write(">>> SET OK: PMD already OFF (0x1), no action required <<<");
    return;
  }

  Enter_BLTN_CAM();
  testWaitForTimeout(3000);

  is_off = Check_Set_PMD_OFF();
  while (is_off == 0 && retry < 15)
  {
    write("  Attempt %d: Force PMD ON edge, then PMD OFF...", retry + 1);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_PMD_NvalueSet = 0x2;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_PMD_NvalueSet = 0x2");
    testWaitForTimeout(500);

    @panel_Rader::LED_Ctrl_Set_PMD = 0;
    testWaitForTimeout(100);
    @panel_Rader::LED_Ctrl_Set_PMD = 1;
    testWaitForTimeout(200);
    @panel_Rader::LED_Ctrl_Set_PMD = 0;
    testWaitForTimeout(500);

    _HU_BLTN_CAM_01_00ms.BLTN_CAM_PMD_NvalueSet = 0x1;
    output(_HU_BLTN_CAM_01_00ms);
    write("  Sent BLTN_CAM_PMD_NvalueSet = 0x1");

    testWaitForTimeout(3000);
    is_off = Check_Set_PMD_OFF();
    retry++;
  }

  if (is_off == 1)
    write(">>> SUCCESS: PMD = OFF (0x1) <<<");
  else
    write(">>> FAILED: Could not set PMD to OFF <<<");

  Exit_BLTN_CAM();
  testWaitForTimeout(1000);
}

// ---------------------------------------------------------------------------
// PMD WaitFor Recording (Polling with Timeout)
// ---------------------------------------------------------------------------

/*
 * WaitFor_RecSta_PMD_Recording(long timeout_ms)
 * Wait for PMD (Parking Motion Detection) recording to start.
 * Signal: BLTN_CAM_RecSta_PMD == 0x1
 * Keywords: wait PMD, PMD recording start, parking motion detection wait
 */
int WaitFor_RecSta_PMD_Recording(long timeout_ms)
{
  long elapsed = 0;
  long log_interval;

  Send_Value_To_COM(8);
  testWaitForTimeout(5000);
  Send_Value_To_COM(8);
  testWaitForTimeout(5000);
  Send_Value_To_COM(8);
  testWaitForTimeout(5000);
  SetLogMessage("DI chuyển để PMD NGAY BAY GIO!");
  write("=== Waiting for PMD Recording (max %d seconds) ===",
        timeout_ms / 1000);

  log_interval = (timeout_ms / 6 / 2000) * 2000;
  if (log_interval < 2000)
    log_interval = 2000;

  while (elapsed < timeout_ms)
  {
    if ($BLTN_CAM_RecSta_PMD == 0x1)
    {
      write(">>> DETECTED: PMD Recording ON after %d seconds <<<",
            elapsed / 1000);
      SetLogMessage("");
      return 1;
    }
    if (elapsed == 0 || elapsed % log_interval == 0)
      write("  [%d s] RecSta_PMD=0x%X", elapsed / 1000, $BLTN_CAM_RecSta_PMD);

    testWaitForTimeout(500);
    elapsed += 2000;
  }
  write(">>> TIMEOUT: PMD Recording NOT detected <<<");
  SetLogMessage("");
  return 0;
}

/*
 * WaitFor_RecSta_PMD_OFF(long timeout_ms)
 * Wait for PMD recording to stop.
 * Signal: BLTN_CAM_RecSta_PMD == 0x0
 * Keywords: wait PMD off, PMD recording stop, parking motion detection stop
 */
int WaitFor_RecSta_PMD_OFF(long timeout_ms)
{
  long elapsed = 0;
  long log_interval;

  write("=== Waiting for PMD Recording OFF (max %d seconds) ===",
        timeout_ms / 1000);

  log_interval = (timeout_ms / 6 / 2000) * 2000;
  if (log_interval < 2000)
    log_interval = 2000;

  while (elapsed < timeout_ms)
  {
    if ($BLTN_CAM_RecSta_PMD == 0x2)
    {
      write(">>> DETECTED: PMD Recording OFF after %d seconds <<<",
            elapsed / 1000);
      return 1;
    }
    if (elapsed == 0 || elapsed % log_interval == 0)
      write("  [%d s] RecSta_PMD=0x%X", elapsed / 1000, $BLTN_CAM_RecSta_PMD);

    testWaitForTimeout(2000);
    elapsed += 2000;
  }
  write(">>> TIMEOUT: PMD Recording still active <<<");
  return 0;
}

// =============================================================================
// SECTION 21: NETWORK MANAGEMENT (CGW NM Control)
// =============================================================================
//
// Purpose: Control CGW Network Management switch via panel variable.
// Keywords: NM, network management, CGW NM, Switch_CGWNM, CAN wake-up,
//           panel control, bus active, bus sleep
//
// Panel variable: @panel::Switch_CGWNM
//   0 = OFF (CAN bus inactive / sleep)
//   1 = ON  (CAN bus active / awake)
//
// Typical flow: NM_ON() -> [key/signal operations] -> NM_OFF()
// =============================================================================

/*
 * Check_NM()
 * ----------
 * Reads the current CGW Network Management state from the panel variable.
 *
 * Keywords: check NM, NM status, CGW NM state, network management, Switch_CGWNM
 * Panel variable read: @panel::Switch_CGWNM
 * Return: 1 = NM is ON, 0 = NM is OFF
 */
int Check_NM()
{
  int state;
  state = @panel::Switch_CGWNM;
  if (state == 1)
    write("  [Check_NM] Switch_CGWNM = 1 (ON)");
  else
    write("  [Check_NM] Switch_CGWNM = 0 (OFF)");
  return state;
}

/*
 * SET_NM_ON()
 * -----------
 * Enables CGW Network Management: sets Switch_CGWNM = 1, waits 2s for
 * network stabilization, then verifies state via Check_NM().
 *
 * Keywords: NM on, network management on, CGW NM enable, CAN wake-up,
 *           bus active, panel Switch_CGWNM
 * Panel variable: @panel::Switch_CGWNM = 1
 * Return: void
 */
void SET_NM_ON()
{
  write("=== NM ON: Enabling CGW Network Management ===");
  @panel::Switch_CGWNM = 1;
  testWaitForTimeout(2000);
  if (Check_NM() == 1)
    write(">>> SUCCESS: NM is ON <<<");
  else
    write(">>> WARNING: NM state unexpected after set <<<");
}

/*
 * SET_NM_OFF()
 * ------------
 * Disables CGW Network Management: sets Switch_CGWNM = 0, waits 1s,
 * then verifies state via Check_NM().
 *
 * Keywords: NM off, network management off, CGW NM disable, CAN sleep,
 *           bus release, panel Switch_CGWNM
 * Panel variable: @panel::Switch_CGWNM = 0
 * Return: void
 */
void SET_NM_OFF()
{
  write("=== NM OFF: Disabling CGW Network Management ===");
  @panel::Switch_CGWNM = 0;
  testWaitForTimeout(1000);
  if (Check_NM() == 0)
    write(">>> SUCCESS: NM is OFF <<<");
  else
    write(">>> WARNING: NM state unexpected after set <<<");
}

/*
 * WaitForNetworkSleep(long timeout_s)
 * -----------------------------------
 * Waits for the network to go to sleep by checking if NM_BLTN_CAM_FD
 * is no longer being transmitted. If testWaitForMessage returns 0 (timeout),
 * it means the message is no longer on the bus, and therefore the network is asleep.
 *
 * Parameter: timeout_s - Max total time to wait in seconds
 * Note: testWaitForMessage can return early when NM frame is detected,
 *       so this function enforces timeout by real elapsed time (timeNow).
 * Keywords: network sleep, sleep mode, NM wait, wait for sleep
 * Return: 1 = Network is asleep, 0 = Network did not sleep (Timeout)
 */
long WaitForNetworkSleep(long timeout_s)
{
  int try_count;
  long timeout_ticks;
  long start_ticks;
  long elapsed_ticks;
  long remaining_ticks;
  long wait_ms;
  long elapsed_s;
  long waitResult;

  if (timeout_s <= 0)
    timeout_s = 1;

  SET_NM_OFF(); // Ensure NM is OFF to allow sleep
  Turn_OFF_HU_Clock();
  // CAPL timeNow() uses 10us ticks => 1 second = 100000 ticks
  timeout_ticks = timeout_s * 100000;
  start_ticks = timeNow();
  try_count = 0;

  write("=== Waiting for Network Sleep (max %d seconds) ===", timeout_s);

  while (1)
  {
    elapsed_ticks = timeNow() - start_ticks;
    if (elapsed_ticks >= timeout_ticks)
      break;

    remaining_ticks = timeout_ticks - elapsed_ticks;
    if (remaining_ticks > 300000)
      wait_ms = 3000;
    else
      wait_ms = remaining_ticks / 100;

    if (wait_ms <= 0)
      wait_ms = 1;

    try_count++;
    waitResult = testWaitForMessage(NM_BLTN_CAM_FD, wait_ms);

    if (waitResult == 0)
    {
      elapsed_s = (timeNow() - start_ticks) / 100000;
      write(">>> SUCCESS: Network is ASLEEP (NM_BLTN_CAM_FD not detected after %d seconds) <<<", elapsed_s);
      testWaitForTimeout(10000);
      // SET_NM_ON(); // Restore NM state after sleep detection
      return 1;
    }

    elapsed_s = (timeNow() - start_ticks) / 100000;
    write("  [Wait] Try %d: NM_BLTN_CAM_FD still detected. Elapsed %d/%d s",
          try_count, elapsed_s, timeout_s);

    // Avoid tight spin when message is continuously present.
    testWaitForTimeout(10);
  }

  write(">>> ERROR: Network did not go to sleep (Timeout after %d seconds) <<<", timeout_s);
  // SET_NM_ON(); // Restore NM state if timeout occurs
  return 0;
}

// =============================================================================
// SECTION 22: POWER SUPPLY CONTROL (TL305TP via RS232)
// =============================================================================
//
// Purpose: Control TENMA TL305TP laboratory power supply via RS232 (COM3)
//          for voltage control on Channel 2 (CH2) during hardware tests.
//
// Connection: TL305TP connected to PC via RS232 on COM3 at 9600 baud
// Channel:    CH2 (index 001 in TL305TP addressing)
//
// Functions:
//   Connect_PowerSupply()           - Open COM3, configure 9600 baud, return
//   1=OK / 0=FAIL Disconnect_PowerSupply()        - Close COM3 connection
//   SetVoltage_CH2(double voltage)  - Set CH2 voltage (e.g. 12.0, 9.0, 16.0 V)
//   TurnOn_Output_CH2()             - Enable CH2 output (send "OUTPUT1/n")
//   TurnOff_Output_CH2()            - Disable CH2 output (send "OUTPUT0/n")
//   Set_BPlus_Off()                 - Placeholder for B+ power disconnect
//   Set_Attach_Font()               - Send attach front camera command value 3
//   Set_Detach_Font()               - Send detach front camera command value 4
//   Set_Attach_Rear()               - Send attach rear camera command value 5
//   Set_Detach_Rear()               - Send detach rear camera command value 6
//   Set_SDcard()                    - Send SD card command value 7 and wait 5s
//
// TL305TP command protocol (no trailing line feed required):
//   Voltage set : "SET:001,VS:12.00,IS:5.00/n"  (001 = CH2, /n is embedded
//   terminator) Output ON   : "OUTPUT1/n" Output OFF  : "OUTPUT0/n"
//
// Typical flow:
//   Connect_PowerSupply() -> SetVoltage_CH2(12.0) -> TurnOn_Output_CH2()
//   -> [run test] -> TurnOff_Output_CH2() -> Disconnect_PowerSupply()
// =============================================================================

/*
 * Connect_PowerSupply()
 * ----------------------
 * Opens COM3 and configures RS232 at 9600-8-N-1 for TL305TP communication.
 * Returns 1 on success, 0 on failure.
 *
 * Keywords: power supply, TL305TP, RS232, COM3, connect, open, 9600 baud
 * Return: 1 = connected, 0 = failed
 */
int Connect_PowerSupply()
{
  write("");
  write("=================================================================");
  write("=== Connecting to Power Supply (TL305TP, COM3) ===");
  write("=================================================================");
  if (RS232Open(3))
  {
    if (rs232Configure(3, 9600, 8, 1, 0))
    {
      write(">>> Power Supply connected successfully (COM3, 9600 baud) <<<");
      return 1;
    }
    else
    {
      write(">>> ERROR: RS232 configuration failed on COM3 <<<");
      return 0;
    }
  }
  else
  {
    write(">>> ERROR: Cannot open COM3 <<<");
    write(">>> Check: Power ON, Cable connected, COM3 available <<<");
    return 0;
  }
}

/*
 * Disconnect_PowerSupply()
 * -------------------------
 * Closes the RS232 connection on COM3.
 *
 * Keywords: power supply, TL305TP, RS232, COM3, disconnect, close
 * Return: void
 */
void Disconnect_PowerSupply()
{
  write("=== Disconnecting Power Supply (COM3) ===");
  RS232Close(3);
  write(">>> Power Supply disconnected <<<");
}

/*
 * SetVoltage_CH2(double voltage)
 * --------------------------------
 * Sets Channel 2 output voltage on TL305TP.
 * Command format: "SET:001,VS:12.00,IS:5.00/n"  (001 = CH2, IS fixed at 5A)
 *
 * Keywords: power supply, TL305TP, voltage, CH2, set voltage, RS232
 * Parameters: voltage - target voltage in Volts (e.g. 9.0, 12.0, 16.0)
 * Return: void
 */
void SetVoltage_CH2(double voltage)
{
  char cmd[50];
  byte buf[50];
  int i, length;

  write("=== Setting CH2 Voltage to %.2f V ===", voltage);

  snprintf(cmd, 50, "SET:001,VS:%05.2f,IS:5.00/n", voltage);
  length = strlen(cmd);
  for (i = 0; i < 50; i++)
    buf[i] = 0;
  for (i = 0; i < length; i++)
    buf[i] = cmd[i];

  if (RS232Send(3, buf, length) == 1)
    write("  TX: %s", cmd);
  else
    write("  TX FAILED: %s", cmd);

  testWaitForTimeout(1000);
  write(">>> Voltage command sent: %.2f V <<<", voltage);
}

/*
 * TurnOn_Output_CH2()
 * --------------------
 * Enables CH2 output on TL305TP (sends "OUTPUT1/n").
 *
 * Keywords: power supply, TL305TP, output ON, CH2 on, enable output
 * Return: void
 */
void TurnOn_Output_CH2()
{
  char cmd[20];
  byte buf[20];
  int i, length;

  write("=== Turning ON CH2 Output ===");
  snprintf(cmd, 20, "OUTPUT1/n");
  length = strlen(cmd);
  for (i = 0; i < 20; i++)
    buf[i] = 0;
  for (i = 0; i < length; i++)
    buf[i] = cmd[i];

  if (RS232Send(3, buf, length) == 1)
    write("  TX: OUTPUT1/n");
  else
    write("  TX FAILED: OUTPUT1/n");

  testWaitForTimeout(500);
  write(">>> Output ON command sent <<<");
}

/*
 * TurnOff_Output_CH2()
 * ---------------------
 * Disables CH2 output on TL305TP (sends "OUTPUT0/n").
 *
 * Keywords: power supply, TL305TP, output OFF, CH2 off, disable output
 * Return: void
 */
void TurnOff_Output_CH2()
{
  char cmd[20];
  byte buf[20];
  int i, length;

  write("=== Turning OFF CH2 Output ===");
  snprintf(cmd, 20, "OUTPUT0/n");
  length = strlen(cmd);
  for (i = 0; i < 20; i++)
    buf[i] = 0;
  for (i = 0; i < length; i++)
    buf[i] = cmd[i];

  if (RS232Send(3, buf, length) == 1)
    write("  TX: OUTPUT0/n");
  else
    write("  TX FAILED: OUTPUT0/n");

  testWaitForTimeout(500);
  write(">>> Output OFF command sent <<<");
}

/*
 * Set_BPlus_Off()
 * ----------------
 * Sends B+ power disconnect command via COM.
 * Command value: 1
 */
void Set_BPlus_Off()
{
  Send_Value_To_COM(1);
  testWaitForTimeout(2000);
}

/*
 * Set_BPlus_On()
 * ---------------
 * Sends B+ power connect command via COM.
 * Command value: 2
 */
void Set_BPlus_On()
{
  Send_Value_To_COM(2);
  testWaitForTimeout(2000);
}

/*
 * Set_Attach_Font()
 * ------------------
 * Sends front camera attach command via COM.
 * Command value: 3
 */
void Set_Attach_Font()
{
  write("=== Set Attach Font via COM value 3 ===");
  Send_Value_To_COM(3);
  testWaitForTimeout(2000);
}

/*
 * Set_Detach_Font()
 * ------------------
 * Sends front camera detach command via COM.
 * Command value: 4
 */
void Set_Detach_Font()
{
  write("=== Set Detach Font via COM value 4 ===");
  Send_Value_To_COM(4);
  testWaitForTimeout(2000);
}

/*
 * Set_Attach_Rear()
 * ------------------
 * Sends rear camera attach command via COM.
 * Command value: 5
 */
void Set_Attach_Rear()
{
  write("=== Set Attach Rear via COM value 5 ===");
  Send_Value_To_COM(5);
  testWaitForTimeout(2000);
}

/*
 * Set_Detach_Rear()
 * ------------------
 * Sends rear camera detach command via COM.
 * Command value: 6
 */
void Set_Detach_Rear()
{
  write("=== Set Detach Rear via COM value 6 ===");
  Send_Value_To_COM(6);
  testWaitForTimeout(2000);
}

/*
 * Set_SDcard()
 * -------------
 * Sends SD card command via COM, then waits 5 seconds.
 * Command value: 7
 */
void Set_SDcard()
{
  write("=== Set SD card via COM value 7 ===");
  Send_Value_To_COM(7);
  testWaitForTimeout(5000);
}

/*
 * Set_relayON()
 * -------------
 * Sends the relay ON command via COM.
 * Command value: 9
 */
void Set_relayON()
{
  write("=== Set relay ON via COM value 9 ===");
  Send_Value_To_COM(9);
}

/*
 * Set_relayOFF()
 * --------------
 * Sends the relay OFF command via COM.
 * Command value: 10
 */
void Set_relayOFF()
{
  write("=== Set relay OFF via COM value 10 ===");
  Send_Value_To_COM(10);
}

/*
 * Send_Value_To_COM(int value)
 * ------------------------------
 * Sends an integer value via RS232 as ASCII text terminated by LF.
 * Format sent: "<value>\n" (example: value=1 -> "1\n").
 *
 * Purpose: Reusable helper for Arduino/serial listener tests.
 * Port config: gComPort (default 4), 9600-8-N-1
 *
 * Notes:
 * - Uses global variable gComPort for port number.
 * - Function attempts RS232Open first.
 * - If open fails (e.g. already open), it still tries rs232Configure
 *   and continues when possible.
 *
 * Keywords: COM, RS232, Arduino, send integer, serial TX, ASCII, LF
 * Parameter: value - integer value to send
 * Return: 1 = send success, 0 = failed
 */
int Send_Value_To_COM(int value)
{
  char txStr[16];
  byte sendBuf[16];
  int i;
  long len;

  if (RS232Open(gComPort))
  {
    if (rs232Configure(gComPort, 9600, 8, 1, 0) == 0)
    {
      write(">>> ERROR: Configure COM%d failed <<<", gComPort);
      testWaitForTimeout(100);

      return 0;
    }
  }
  else
  {
    // COM port may already be open; try configure and continue.
    if (rs232Configure(gComPort, 9600, 8, 1, 0) == 0)
    {
      write(">>> ERROR: Cannot open/configure COM%d <<<", gComPort);
      testWaitForTimeout(100);

      return 0;
    }
  }

  snprintf(txStr, elcount(txStr), "%d\n", value);
  len = strlen(txStr);

  for (i = 0; i < len; i++)
    sendBuf[i] = txStr[i];

  if (RS232Send(gComPort, sendBuf, len) == 1)
  {
    write(">>> TX COM%d: %s <<<", gComPort, txStr);
    testWaitForTimeout(100);

    return 1;
  }

  write(">>> TX FAILED COM%d: %s <<<", gComPort, txStr);
  testWaitForTimeout(100);

  return 0;
}

// =============================================================================
// SECTION 23: ENGINEER MODE (LVDS Mode Control via CAN)
// =============================================================================
//
// Purpose: Enter Engineer / LVDS diagnostic mode on the BLTN_CAM unit for
//          factory-level inspection and calibration.
//
// NOTE: _HU_BLTN_CAM_01_00ms message is declared locally within this function.
//       No declaration needed in the calling .can file.
//
// Signals:
//   BLTN_CAM_LVDS_Mode_Cmd   - set to 0x2 to request Engineer Mode
//   BLTN_CAM_LVDS_Mode_State - 0x02 = confirmed in Engineer Mode
//
// Functions:
//   Enter_Engineer_Mode(long timeout_ms)
//       - Sends HU_BLTN_CAM_01_00ms with all bytes = 0xFF and
//         BLTN_CAM_LVDS_Mode_Cmd = 0x2
//       - Polls BLTN_CAM_LVDS_Mode_State == 0x02 every 500 ms
//       - Resends command periodically until confirmed or timeout
//       - Returns 1 = success, 0 = timeout
// =============================================================================

/*
 * Enter_Engineer_Mode(long timeout_ms)
 * --------------------------------------
 * Sends LVDS mode command to enter Engineer Mode and waits for confirmation.
 * Resends the command periodically until BLTN_CAM_LVDS_Mode_State == 0x02.
 *
 * Keywords: engineer mode, LVDS mode, calibration, factory mode, diagnostic
 * Parameters: timeout_ms - maximum wait time in milliseconds
 * Return: 1 = entered Engineer Mode, 0 = timeout
 */
int Enter_Engineer_Mode(long timeout_ms)
{
  long elapsed = 0;
  long log_interval;
  byte lvds_mode = 0;
  int i;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;

  write("=== Entering Engineer Mode via signal ===");

  // Send Engineer Mode command directly
  for (i = 0; i < 8; i++)
  {
    _HU_BLTN_CAM_01_00ms.byte(i) = 0xFF;
  }
  _HU_BLTN_CAM_01_00ms.BLTN_CAM_LVDS_Mode_Cmd = 0x2;
  output(_HU_BLTN_CAM_01_00ms);

  write("Waiting for Engineer Mode confirmation (LVDS_Mode_State = 0x02)...");

  log_interval = (timeout_ms / 6 / 500) * 500;
  if (log_interval < 500)
    log_interval = 500;

  while (elapsed < timeout_ms)
  {
    lvds_mode = $BLTN_CAM_LVDS_Mode_State;

    if (lvds_mode == 0x02)
    {
      write(">>> Now in Engineer Mode! LVDS_Mode_State = 0x02 <<<");
      return 1;
    }

    if (elapsed == 0 || elapsed % log_interval == 0)
    {
      write("  [%d s] LVDS_Mode_State=0x%X (waiting for 0x02)", elapsed / 1000,
            lvds_mode);
      // Resend command
      for (i = 0; i < 8; i++)
      {
        _HU_BLTN_CAM_01_00ms.byte(i) = 0xFF;
      }
      _HU_BLTN_CAM_01_00ms.BLTN_CAM_LVDS_Mode_Cmd = 0x2;
      output(_HU_BLTN_CAM_01_00ms);
    }

    testWaitForTimeout(500);
    elapsed += 500;
  }

  write(">>> Timeout! Could not enter Engineer Mode <<<");
  return 0;
}

// =============================================================================
// SECTION 24: TEST MONITOR PANEL CONTROL
// =============================================================================
//
// Purpose: Control the TestMonitor panel elements (Elapsed Time, Boot Time).
//          Provides background timers that update the panel every second.
//
// System Variables (namespace: TestMonitor):
//   - TestMonitor::TestStepName         (String)
//   - TestMonitor::ElapsedSeconds       (Integer)
//   - TestMonitor::TimerRunning         (Integer)
//   - TestMonitor::CheckTimeBootSeconds (Integer)
//   - TestMonitor::CheckTimeBootRunning (Integer)
//   - TestMonitor::LogMessage           (String)
// =============================================================================

/*
 * SetTestStep(char stepName[])
 * Updates the Test Step Name field on the panel.
 */
void SetTestStep(char stepName[])
{
  sysSetVariableString(sysvar::TestMonitor::TestStepName, stepName);
  write("TestMonitor: Step = %s", stepName);
}

/*
 * SetLogMessage(char logMsg[])
 * Updates the LOG MESSAGE field on the panel with red text.
 */
void SetLogMessage(char logMsg[])
{
  sysSetVariableString(sysvar::TestMonitor::LogMessage, logMsg);
  write("TestMonitor: Log = %s", logMsg);
}

/*
 * StartTestTimer()
 * Resets and starts the Elapsed Time counter.
 */
void StartTestTimer()
{
  gTestMonitorSeconds = 0;
  @sysvar::TestMonitor::ElapsedSeconds = 0;
  @sysvar::TestMonitor::TimerRunning = 1;
  setTimer(gTestMonitorTimer, 1000);
  write("TestMonitor: Elapsed Timer started");
}

/*
 * StopTestTimer()
 * Stops the Elapsed Time counter.
 */
void StopTestTimer()
{
  cancelTimer(gTestMonitorTimer);
  @sysvar::TestMonitor::TimerRunning = 0;
  write("TestMonitor: Elapsed Timer stopped at %d s", gTestMonitorSeconds);
}

/*
 * StartBootTimer()
 * Resets and starts the Boot Time counter.
 */
void StartBootTimer()
{
  gBootMonitorSeconds = 0;
  @sysvar::TestMonitor::CheckTimeBootSeconds = 0;
  @sysvar::TestMonitor::CheckTimeBootRunning = 1;
  setTimer(gBootMonitorTimer, 1);
  write("TestMonitor: Boot Timer started");
}

/*
 * StopBootTimer()
 * Stops the Boot Time counter.
 */
void StopBootTimer()
{
  cancelTimer(gBootMonitorTimer);
  @sysvar::TestMonitor::CheckTimeBootRunning = 0;
  write("TestMonitor: Boot Timer stopped at %d s", gBootMonitorSeconds);
}

// ---------------------------------------------------------------------------
// Timer Event Handlers
// ---------------------------------------------------------------------------

on timer gTestMonitorTimer
{
  gTestMonitorSeconds++;
  @sysvar::TestMonitor::ElapsedSeconds = gTestMonitorSeconds;
  setTimer(gTestMonitorTimer, 1000);
}

on timer gBootMonitorTimer
{
  gBootMonitorSeconds++;
  @sysvar::TestMonitor::CheckTimeBootSeconds = gBootMonitorSeconds;
  setTimer(gBootMonitorTimer, 1);
}

// =============================================================================
// MERGED FROM INF/DeclarationFunction_Gen2.5.h
// =============================================================================

// Declaration Function
on timer send_BDC_FD_SMK_03_200ms_timer
{
  _BDC_FD_SMK_03_200ms.SMK_TrmnlCtrlGrpStaBDC = KEY_STATE;
  output(_BDC_FD_SMK_03_200ms);
  send_BDC_FD_SMK_03_200ms_timer.set(200);
}

void CaptureGraphics_Lft()
{
  testWaitForTimeout(3000);
  testReportAddWindowCapture("INF", "BLTN_CAM_RecSta_OWD; Lamp_TrnSigLmpLftOnReq", "Sreenshot", "./Husign.jpg");
}

void CaptureGraphics_Rt()
{
  testWaitForTimeout(3000);
  testReportAddWindowCapture("INF", "BLTN_CAM_RecSta_OWD; Lamp_TrnSigLmpRtOnReq", "Sreenshot", "./Husign.jpg");
}

void Init()
{

  @panel::Switch_CGWNM = 1;

  KEY_STATE = 0x1;
  testWaitForTimeout(3000);

  if ($BLTN_CAM_RecSet_OWD == 0x2 || $BLTN_CAM_RecSet_OWP == 0x2 || $BLTN_CAM_RecSet_DEV == 0x2 || $BLTN_CAM_RecSet_PEV == 0x2)
  {

    do
    {
      if ($BLTN_CAM_RecSet_OWD == 0x2)
      {
        write("OWD OFF");
        Turn_OFF_OWD();
      }
      else if ($BLTN_CAM_RecSet_OWP == 0x2)
      {
        write("OWP OFF");
        Turn_OFF_OWP();
      }
      else if ($BLTN_CAM_RecSet_DEV == 0x2)
      {
        write("DEV OFF");
        Turn_OFF_DEV();
      }
      else if ($BLTN_CAM_RecSet_PEV == 0x2)
      {
        write("PEV OFF");
        Turn_OFF_PEV();
      }
      testWaitForTimeout(2000);
      flag_RecSet_error++;
    } while (($BLTN_CAM_RecSet_OWD == 0x2 || $BLTN_CAM_RecSet_OWP == 0x2 || $BLTN_CAM_RecSet_DEV == 0x2 || $BLTN_CAM_RecSet_PEV == 0x2) && (flag_RecSet_error < 15));

    flag_RecSet_error = 0;
  }
  else
  {
    write("Notifications: All recording setting are turned OFF.");
  }

  write("Reset Recorded Video");
  @panel::Button_RecReset = 1;

  KEY_STATE = 0x0;
  @panel::Switch_CGWNM = 0;
}

void BLTN_CAM_Settings()
{
  if ($BLTN_CAM_RecSet_OWP == 0x1)
  {
    write("OWP On");
    Turn_ON_OWP();
  }
}

// Check lo?i xe
int check_Model_Type(char a[], int size)
{
  int len;
  len = my_strlen(a);

  if (len == 3 && a[0] == 'I' && a[1] == 'C' && a[2] == 'E')
    return 1;
  if (len == 3 && a[0] == 'H' && a[1] == 'E' && a[2] == 'V')
    return 2;
  if (len == 2 && a[0] == 'E' && a[1] == 'V')
    return 3;

  return 0;
}

/* helper: clear chu?i */
void clearString(char arr[], int size)
{
  int i;
  for (i = 0; i < size; i++)
    arr[i] = '\0';
}

/* helper: tr? v? d? di chu?i (simple strlen) */
int my_strlen(char s[])
{
  int i = 0;
  while (s[i] != '\0')
    i++;
  return i;
}

// =============================================================================
// SECTION 25: STAMP NvalueSet FUNCTIONS (Driving Info stamps via CAN FD)
// =============================================================================
//
// Purpose: Send individual or all stamp ON/OFF commands via HU_BLTN_CAM_01_00ms
//          and verify the corresponding Set signal.
//
// Pattern: Send NvalueSet signal -> wait -> check Set signal == value
//          Retry up to 10 times if mismatch.
//
// Return: 1 = success, 0 = failed after retries
// =============================================================================

/*
 * Send_Tim_Stm_NvalueSet(int value)
 * Send Time Stamp NvalueSet command and verify.
 * Signal: BLTN_CAM_Time_Stm_NvalueSet -> check BLTN_CAM_Set_Tim_Stm
 */
int Send_Tim_Stm_NvalueSet(int value)
{
  int retry;
  message HU_BLTN_CAM_01_00ms _msg;

  write("=== Send_Tim_Stm_NvalueSet(0x%X) ===", value);

  for (retry = 0; retry < 10; retry++)
  {
    _msg.BLTN_CAM_Time_Stm_NvalueSet = value;
    output(_msg);
    testWaitForTimeout(2000);

    if ($BLTN_CAM_Set_Tim_Stm == value)
    {
      write(">>> [OK] BLTN_CAM_Set_Tim_Stm = 0x%X (attempt %d) <<<", value, retry + 1);
      return 1;
    }
    write("  [Retry %d/10] Set_Tim_Stm=0x%X (expected 0x%X)", retry + 1, $BLTN_CAM_Set_Tim_Stm, value);
  }

  write(">>> [NG] Failed to set Tim_Stm after 10 retries <<<");
  return 0;
}

/*
 * Send_Spd_Stm_NvalueSet(int value)
 * Send Speed Stamp NvalueSet command and verify.
 * Signal: BLTN_CAM_Spd_Stm_NvalueSet -> check BLTN_CAM_Set_Spd_Stm
 */
int Send_Spd_Stm_NvalueSet(int value)
{
  int retry;
  message HU_BLTN_CAM_01_00ms _msg;

  write("=== Send_Spd_Stm_NvalueSet(0x%X) ===", value);

  for (retry = 0; retry < 10; retry++)
  {
    _msg.BLTN_CAM_Spd_Stm_NvalueSet = value;
    output(_msg);
    testWaitForTimeout(2000);

    if ($BLTN_CAM_Set_Spd_Stm == value)
    {
      write(">>> [OK] BLTN_CAM_Set_Spd_Stm = 0x%X (attempt %d) <<<", value, retry + 1);
      return 1;
    }
    write("  [Retry %d/10] Set_Spd_Stm=0x%X (expected 0x%X)", retry + 1, $BLTN_CAM_Set_Spd_Stm, value);
  }

  write(">>> [NG] Failed to set Spd_Stm after 10 retries <<<");
  return 0;
}

/*
 * Send_GearPos_Stm_NvalueSet(int value)
 * Send Gear Position Stamp NvalueSet command and verify.
 * Signal: BLTN_CAM_GearPos_Stm_NvalueSet -> check BLTN_CAM_Set_GearPos_Stm
 */
int Send_GearPos_Stm_NvalueSet(int value)
{
  int retry;
  message HU_BLTN_CAM_01_00ms _msg;

  write("=== Send_GearPos_Stm_NvalueSet(0x%X) ===", value);

  for (retry = 0; retry < 10; retry++)
  {
    _msg.BLTN_CAM_GearPos_Stm_NvalueSet = value;
    output(_msg);
    testWaitForTimeout(2000);

    if ($BLTN_CAM_Set_GearPos_Stm == value)
    {
      write(">>> [OK] BLTN_CAM_Set_GearPos_Stm = 0x%X (attempt %d) <<<", value, retry + 1);
      return 1;
    }
    write("  [Retry %d/10] Set_GearPos_Stm=0x%X (expected 0x%X)", retry + 1, $BLTN_CAM_Set_GearPos_Stm, value);
  }

  write(">>> [NG] Failed to set GearPos_Stm after 10 retries <<<");
  return 0;
}

/*
 * Send_TrnSig_Stm_NvalueSet(int value)
 * Send Turn Signal Stamp NvalueSet command and verify.
 * Signal: BLTN_CAM_TrnSig_Stm_NvalueSet -> check BLTN_CAM_Set_TrnSig_Stm
 */
int Send_TrnSig_Stm_NvalueSet(int value)
{
  int retry;
  message HU_BLTN_CAM_01_00ms _msg;

  write("=== Send_TrnSig_Stm_NvalueSet(0x%X) ===", value);

  for (retry = 0; retry < 10; retry++)
  {
    _msg.BLTN_CAM_TrnSig_Stm_NvalueSet = value;
    output(_msg);
    testWaitForTimeout(2000);

    if ($BLTN_CAM_Set_TrnSig_Stm == value)
    {
      write(">>> [OK] BLTN_CAM_Set_TrnSig_Stm = 0x%X (attempt %d) <<<", value, retry + 1);
      return 1;
    }
    write("  [Retry %d/10] Set_TrnSig_Stm=0x%X (expected 0x%X)", retry + 1, $BLTN_CAM_Set_TrnSig_Stm, value);
  }

  write(">>> [NG] Failed to set TrnSig_Stm after 10 retries <<<");
  return 0;
}

/*
 * Send_EVSen_Stm_NvalueSet(int value)
 * Send Event Sensor Stamp NvalueSet command and verify.
 * Signal: BLTN_CAM_EVSen_Stm_NvalueSet -> check BLTN_CAM_Set_EVSen_Stm
 */
int Send_EVSen_Stm_NvalueSet(int value)
{
  int retry;
  message HU_BLTN_CAM_01_00ms _msg;

  write("=== Send_EVSen_Stm_NvalueSet(0x%X) ===", value);

  for (retry = 0; retry < 10; retry++)
  {
    _msg.BLTN_CAM_EVSen_Stm_NvalueSet = value;
    output(_msg);
    testWaitForTimeout(2000);

    if ($BLTN_CAM_Set_EVSen_Stm == value)
    {
      write(">>> [OK] BLTN_CAM_Set_EVSen_Stm = 0x%X (attempt %d) <<<", value, retry + 1);
      return 1;
    }
    write("  [Retry %d/10] Set_EVSen_Stm=0x%X (expected 0x%X)", retry + 1, $BLTN_CAM_Set_EVSen_Stm, value);
  }

  write(">>> [NG] Failed to set EVSen_Stm after 10 retries <<<");
  return 0;
}

/*
 * Send_Locatn_Stm_NvalueSet(int value)
 * Send Location Stamp NvalueSet command and verify.
 * Signal: BLTN_CAM_Locatn_Stm_NvalueSet -> check BLTN_CAM_Set_Locatn_Stm
 */
int Send_Locatn_Stm_NvalueSet(int value)
{
  int retry;
  message HU_BLTN_CAM_01_00ms _msg;

  write("=== Send_Locatn_Stm_NvalueSet(0x%X) ===", value);

  for (retry = 0; retry < 10; retry++)
  {
    _msg.BLTN_CAM_Locatn_Stm_NvalueSet = value;
    output(_msg);
    testWaitForTimeout(2000);

    if ($BLTN_CAM_Set_Locatn_Stm == value)
    {
      write(">>> [OK] BLTN_CAM_Set_Locatn_Stm = 0x%X (attempt %d) <<<", value, retry + 1);
      return 1;
    }
    write("  [Retry %d/10] Set_Locatn_Stm=0x%X (expected 0x%X)", retry + 1, $BLTN_CAM_Set_Locatn_Stm, value);
  }

  write(">>> [NG] Failed to set Locatn_Stm after 10 retries <<<");
  return 0;
}

/*
 * Send_All_Stm_NvalueSet(int value)
 * Send ALL stamp NvalueSet commands (Tim, Spd, GearPos, TrnSig, EVSen, Locatn)
 * and verify each one. Returns 1 only if ALL succeed.
 */
int Send_All_Stm_NvalueSet(int value)
{
  int all_ok = 1;

  write("=== Send_All_Stm_NvalueSet(0x%X) ===", value);

  if (Send_Tim_Stm_NvalueSet(value) == 0)
  {
    all_ok = 0;
    write("  [NG] Tim_Stm failed");
  }
  if (Send_Spd_Stm_NvalueSet(value) == 0)
  {
    all_ok = 0;
    write("  [NG] Spd_Stm failed");
  }
  if (Send_GearPos_Stm_NvalueSet(value) == 0)
  {
    all_ok = 0;
    write("  [NG] GearPos_Stm failed");
  }
  if (Send_TrnSig_Stm_NvalueSet(value) == 0)
  {
    all_ok = 0;
    write("  [NG] TrnSig_Stm failed");
  }
  if (Send_EVSen_Stm_NvalueSet(value) == 0)
  {
    all_ok = 0;
    write("  [NG] EVSen_Stm failed");
  }
  if (Send_Locatn_Stm_NvalueSet(value) == 0)
  {
    all_ok = 0;
    write("  [NG] Locatn_Stm failed");
  }

  if (all_ok == 1)
    write(">>> [OK] All stamps set to 0x%X <<<", value);
  else
    write(">>> [NG] Some stamps failed to set <<<");

  return all_ok;
}

void Choose_ModelType()
{
  int mode_type = 0;

  do
  {
    clearString(answer, 4);

    testWaitForStringInput("Enter model type car (ICE/HEV/EV)", 300000);
    testGetStringInput(answer, 4);

    write("Your input: '%s'", answer);

    mode_type = check_Model_Type(answer, 4);

    if (mode_type == 0)
    {
      write("Invalid input! Please enter ICE or HEV or EV");
    }

    clearString(answer, 4);

  } while (mode_type == 0);

  switch (mode_type)
  {
  case (1):
    @panel::RadioBtn_Vehicle_Type = 0;
    break;
  case (2):
    @panel::RadioBtn_Vehicle_Type = 1;
    break;
  case (3):
    @panel::RadioBtn_Vehicle_Type = 2;
    break;
  }

  @panel::EnableDrvInfo = 1;
}

void Set_Off_Stamp()
{
  if ($BLTN_CAM_Set_EVSen_Stm == 0x2)
  {
    write(" Set EVSen Stamp Off");
    @panel::LED_Ctrl_Set_EVSen_Stm = 1;
    testWaitForTimeout(500);
  };
  if ($BLTN_CAM_Set_GearPos_Stm == 0x2)
  {
    write("Set GearPos Stamp Off");
    @panel::LED_Ctrl_Set_GearPos_Stm = 1;
    testWaitForTimeout(500);
  }
  if ($BLTN_CAM_Set_Locatn_Stm == 0x2)
  {
    write("Set Locatn Stamp Off");
    @panel::LED_Ctrl_Set_Locatn_Stm = 1;
    testWaitForTimeout(500);
  }
  if ($BLTN_CAM_Set_Spd_Stm == 0x2)
  {
    write("Set Speed Stamp Off");
    @panel::LED_Ctrl_Set_Spd_Stm = 1;
    testWaitForTimeout(500);
  }
  if ($BLTN_CAM_Set_Tim_Stm == 0x2)
  {
    write("Set Time Stamp Off");
    @panel::LED_Ctrl_Set_Tim_Stm = 1;
    testWaitForTimeout(500);
  }
  if ($BLTN_CAM_Set_TrnSig_Stm == 0x2)
  {
    write("Set TrnSig Stamp Off");
    @panel::LED_Ctrl_Set_TrnSig_Stm = 1;
    testWaitForTimeout(500);
  }
}

void Choose_CarModel()
{
  int mode_type = 0;

  do
  {
    // clear tru?c khi d?c (an ton)
    clearString(answer, 4);

    testWaitForStringInput("Enter model type car (ICE/HEV/EV)", 300000);
    testGetStringInput(answer, 4); // d?c t?i da 3 k t? (c?ng '\0')

    write("Your input: '%s'", answer);

    mode_type = check_Model_Type(answer, 4);

    if (mode_type == 0)
    {
      write("Invalid input! Please enter ICE or HEV or EV");
    }

    // clear tru?c khi l?p l?i (khng b?t bu?c n?a nhung an ton)
    clearString(answer, 4);

  } while (mode_type == 0);

  switch (mode_type)
  {
  case (1):
    @panel::RadioBtn_Vehicle_Type = 0;
    break;
  case (2):
    @panel::RadioBtn_Vehicle_Type = 1;
    break;
  case (3):
    @panel::RadioBtn_Vehicle_Type = 2;
    break;
  }

  @panel::EnableDrvInfo = 1;
}
