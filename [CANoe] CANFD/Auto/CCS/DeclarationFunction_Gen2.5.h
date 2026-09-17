/*@!Encoding:1252*/
// Declaration Function
  void KEY_ON(){
    setSignal(SMK_TrmnlCtrlGrpStaBDC, 0x2); //IGN
  }
  
  void KEY_OFF(){
    setSignal(SMK_TrmnlCtrlGrpStaBDC, 0x0); //OFF
  }
  
  void Enter_BLTN_CAM(){
    // CAN FD
    _HU_BLTN_CAM_02_200ms.HU_BLTN_CAM_UI_Mode = 0x1; //Enter BLTN_CAM mode  
    output(_HU_BLTN_CAM_02_200ms);
  }
  
  void Exit_BLTN_CAM(){
    // CAN FD
    _HU_BLTN_CAM_02_200ms.HU_BLTN_CAM_UI_Mode = 0x0; //Enter AVN mode - Exit BLTN_CAM Mode
    output(_HU_BLTN_CAM_02_200ms);  
  }
  
  void Set_All_Recording_OFF(){
      do {
      Enter_BLTN_CAM();
      testWaitForTimeout(3000);
      if(getSignal(BLTN_CAM_RecSet_OWD) == 0x2){  
        @panel::LED_Ctrl_SetOWD = 1;
        write("OWD OFF");
        testWaitForTimeout(500);
      }
      
      if(getSignal(BLTN_CAM_RecSet_OWP) == 0x2){      
        @panel::LED_Ctrl_SetOWP = 1;
        write("OWP OFF");
        testWaitForTimeout(500);
      }
      
      if(getSignal(BLTN_CAM_RecSet_DEV) == 0x2){   
        @panel::LED_Ctrl_SetDEV = 1;
        write("DEV OFF");
        testWaitForTimeout(500);
      }
      if(getSignal(BLTN_CAM_RecSet_PEV) == 0x2){   
        @panel::LED_Ctrl_SetPEV = 1;
        write("PEV OFF");
        testWaitForTimeout(500);
      }  
      flag_RecSet_error++;
        
      Exit_BLTN_CAM();
        
      } while ((getSignal(BLTN_CAM_RecSet_OWD) == 0x2 || getSignal(BLTN_CAM_RecSet_OWP) == 0x2 || getSignal(BLTN_CAM_RecSet_DEV) == 0x2 || getSignal(BLTN_CAM_RecSet_PEV) == 0x2) && (flag_RecSet_error < 15));
      
      flag_RecSet_error = 0;
  }
  
  void Init() {
    
    flag = 0;
    flag_RecSet_error = 0;
    condition1 = 0; condition2 = 0; condition3 = 0;
    KEY_state = 0x0;
    Profile_value = 0x1;
    Mode_state = 0x2;
    Bat_val = 0x50;
    Alarm_state = 0x0; 
    
    testWaitForTimeout(1000);
  
    send_IAU_ProfileIDRVal_timer.cancel();
    send_SMK_TrmnlCtrlGrpStaBDC_timer.cancel();
    send_ICU_PowerAutoCutModSta_timer.cancel();
    send_ICU_BS2SoC_timer.cancel();
    send_BAlarm_BglrAlrmCurrentSta_timer.cancel();
  
  }
  
  void Begin() {
    
    send_SMK_TrmnlCtrlGrpStaBDC_timer.set(10);
    send_IAU_ProfileIDRVal_timer.set(10);
    send_ICU_PowerAutoCutModSta_timer.set(10);
    send_ICU_BS2SoC_timer.set(10);
    send_BAlarm_BglrAlrmCurrentSta_timer.set(10);    
  
  }