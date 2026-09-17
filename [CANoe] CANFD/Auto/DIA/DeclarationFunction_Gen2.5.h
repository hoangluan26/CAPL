/*@!Encoding:1252*/
// Declaration Function for OWD Test Cases

variables
{
  // Global Variable Declaration
  int flag_RecSet_error = 0;
  
  byte time_Hour_value = 0x17;
  byte time_Minute_value = 0x3A;
  byte time_Second_value = 0x1C;
  
  byte check_DisplayYear2_value;
  byte check_DisplayYear1_value;
  byte check_DisplayMonth_value;
  byte check_DisplayDay_value;
  
  byte value_Bfr = 0x0;
  byte value_Aft = 0x0;
  
  float count_DEVRec_time = 0.0;
  
  // Message Declaration for CAN FD
  message HU_BLTN_CAM_02_200ms _HU_BLTN_CAM_02_200ms;
  message HU_BLTN_CAM_01_00ms _HU_BLTN_CAM_01_00ms;
  message HU_CLOCK_01_1000ms _HU_CLOCK_01_1000ms;
}

void KEY_ON()
{
  setSignal(SMK_TrmnlCtrlGrpStaBDC, 0x1); // Power ON
}

void KEY_OFF()
{
  setSignal(SMK_TrmnlCtrlGrpStaBDC, 0x0); // Power OFF
}

void Enter_BLTN_CAM()
{
  // CAN FD
  _HU_BLTN_CAM_02_200ms.HU_BLTN_CAM_UI_Mode = 0x1; //Enter BLTN_CAM mode
  output(_HU_BLTN_CAM_02_200ms);
}

void Exit_BLTN_CAM()
{
  // CAN FD
  _HU_BLTN_CAM_02_200ms.HU_BLTN_CAM_UI_Mode = 0x0; //Enter AVN mode - Exit BLTN_CAM Mode
  output(_HU_BLTN_CAM_02_200ms);
}

void Check_time()
{
  check_DisplayYear2_value = getSignal(HU_DisplayYear2);
  check_DisplayYear1_value = getSignal(HU_DisplayYear1);
  check_DisplayMonth_value = getSignal(HU_DisplayMonth);
  check_DisplayDay_value = getSignal(HU_DisplayDay);
}

void CaptureGraphics(char windowName[], char graphicName[])
{
  testWaitForTimeout(3000);
  testReportAddWindowCapture(windowName, graphicName, "Screenshot", "./Husign.jpg");
}

void Clock_Down(long CD_time)
{
  write("=== COUNT DOWN CLOCK ===");
  while (CD_time > 0) {
    write("Remaining time: 00::%02d:%02d", CD_time / (1000 * 60), (CD_time / 1000) % 60);
    testWaitForTimeout(1000);
    CD_time -= 1000;
  }
}

void Set_Time()
{
  @panel::Switch_Clock = 1;
  testWaitForTimeout(2000);
  
  Check_time();
  testWaitForTimeout(1000);
  
  @panel::Switch_Clock = 0;
  
  // CAN FD
  _HU_CLOCK_01_1000ms.HU_TimeFormat = 0x2;
  _HU_CLOCK_01_1000ms.HU_Status = 0x1;
  _HU_CLOCK_01_1000ms.HU_TimeSetting = 0x1;
  _HU_CLOCK_01_1000ms.HU_DisplayHour = time_Hour_value;
  _HU_CLOCK_01_1000ms.HU_DisplayMinute = time_Minute_value;
  _HU_CLOCK_01_1000ms.HU_DisplaySecond = time_Second_value;
  _HU_CLOCK_01_1000ms.HU_DisplayYear1 = check_DisplayYear1_value;
  _HU_CLOCK_01_1000ms.HU_DisplayYear2 = check_DisplayYear2_value;
  _HU_CLOCK_01_1000ms.HU_DisplayMonth = check_DisplayMonth_value;
  _HU_CLOCK_01_1000ms.HU_DisplayDay = check_DisplayDay_value;
  _HU_CLOCK_01_1000ms.HU_ClockInfo = 0x1; //ACLOCK Refresh
  _HU_CLOCK_01_1000ms.HU_ComRefresh = 0x2; //Internal Clock Exist
  _HU_CLOCK_01_1000ms.HU_AM_PM_INFO = 0x0; //Not used
  output(_HU_CLOCK_01_1000ms);
}

void Turn_OFF_RecSet()
{  
  // Dọn sạch chế độ ghi hình để đảm bảo test độc lập, không bị nhiễu trạng thái cũ
  if (getSignal(BLTN_CAM_RecSet_OWD) == 0x2 || getSignal(BLTN_CAM_RecSet_OWP) == 0x2 || 
      getSignal(BLTN_CAM_RecSet_DEV) == 0x2 || getSignal(BLTN_CAM_RecSet_PEV) == 0x2) {
    @panel::Switch_CGWNM = 1;
    
    KEY_ON();
    testWaitForTimeout(3000);
    
    do {
      Enter_BLTN_CAM();
      testWaitForTimeout(3000);
      
      if (getSignal(BLTN_CAM_RecSet_OWD) == 0x2) {
        write("OWD Off");
        @panel::LED_Ctrl_SetOWD = 1;
        testWaitForTimeout(1000);
      } else if (getSignal(BLTN_CAM_RecSet_OWP) == 0x2) {
        write("OWP Off");
        @panel::LED_Ctrl_SetOWP = 1;
        testWaitForTimeout(1000);
      } else if (getSignal(BLTN_CAM_RecSet_DEV) == 0x2) {
        write("DEV Off");
        @panel::LED_Ctrl_SetDEV = 1;
        testWaitForTimeout(1000);
      } else if (getSignal(BLTN_CAM_RecSet_PEV) == 0x2) {
        write("PEV Off");
        @panel::LED_Ctrl_SetPEV = 1;
        testWaitForTimeout(1000);
      }
      
      Exit_BLTN_CAM();
      
      flag_RecSet_error++;
      
    } while ((getSignal(BLTN_CAM_RecSet_OWD) == 0x2 || getSignal(BLTN_CAM_RecSet_OWP) == 0x2 || 
              getSignal(BLTN_CAM_RecSet_DEV) == 0x2 || getSignal(BLTN_CAM_RecSet_PEV) == 0x2) && 
             (flag_RecSet_error <= 15));
    
    flag_RecSet_error = 0;
    testWaitForTimeout(3000);
    KEY_OFF();
  } else {
    write("Notifications: All recording settings are turned OFF.");
  }
}

void Init()
{
  // Đảm bảo thẻ được Format, hệ thống sạch sẽ
  @panel::Switch_CGWNM = 1;
  
  KEY_ON();
  
  while (getSignal(BLTN_CAM_SD_Format_Op_State) != 0x1) {
    @panel::Button_RecReset = 1;
    testWaitForSignalChange(BLTN_CAM_SD_Format_Op_State, 500);
  }
  
  KEY_OFF();
  
  @panel::Switch_CGWNM = 0;
}
