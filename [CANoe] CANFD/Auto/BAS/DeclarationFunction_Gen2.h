/*@!Encoding:1252*/
// Declaration Function
void KEY_ON(){
    setSignal(SMK_TrmnlCtrlGrpStaBDC, 0x2); //IGN
  }
  
  void KEY_OFF(){
    setSignal(SMK_TrmnlCtrlGrpStaBDC, 0x0); //OFF
  }
  
  void Enter_BLTN_CAM(){
    // CAN
    _HU_BLTN_CAM_PE_00_200ms.HU_BLTN_CAM_UI_Mode=0x1;
    output(_HU_BLTN_CAM_PE_00_200ms); //Enter BLTN_CAM mode    
  }
  
  void Exit_BLTN_CAM(){
    // CAN
    _HU_BLTN_CAM_PE_00_200ms.HU_BLTN_CAM_UI_Mode=0x0;
    output(_HU_BLTN_CAM_PE_00_200ms);//Exit BLTN_CAM Mode      
  }
  
  void BLTN_CAM_Settings(){
    // CAN
    // Recording Settings
    _HU_BLTN_CAM_E_00.BLTN_CAM_Rec_OWD_NvalueSet = arr_random_setting[0];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);  
    _HU_BLTN_CAM_E_00.BLTN_CAM_Rec_OWP_NvalueSet = arr_random_setting[1];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);   
    _HU_BLTN_CAM_E_00.BLTN_CAM_Rec_DEV_NvalueSet = arr_random_setting[2];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);   
    _HU_BLTN_CAM_E_00.BLTN_CAM_Rec_PEV_NvalueSet = arr_random_setting[3];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);   
    _HU_BLTN_CAM_E_00.BLTN_CAM_EV_RecMaxTime_NvalueSet = arr_random_setting[4];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);   
    // Time Settings
    _HU_BLTN_CAM_E_00.BLTN_CAM_PrkTime_NvalueSet = arr_random_setting[5];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);   
    _HU_BLTN_CAM_E_00.BLTN_CAM_TML_Int_NvalueSet = arr_random_setting[6];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);   
    _HU_BLTN_CAM_E_00.BLTN_CAM_EV_BfrTime_NvalueSet = arr_random_setting[7];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);   
    _HU_BLTN_CAM_E_00.BLTN_CAM_EV_AftTime_NvalueSet = arr_random_setting[8];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);   
    // Stamp Information Settings 
    _HU_BLTN_CAM_E_00.BLTN_CAM_EVSen_Stm_NvalueSet = arr_random_setting[9];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);   
    _HU_BLTN_CAM_E_00.BLTN_CAM_GearPos_Stm_NvalueSet = arr_random_setting[10];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);   
    _HU_BLTN_CAM_E_00.BLTN_CAM_Locatn_Stm_NvalueSet = arr_random_setting[11];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);   
    _HU_BLTN_CAM_E_00.BLTN_CAM_Spd_Stm_NvalueSet = arr_random_setting[12];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);   
    _HU_BLTN_CAM_E_00.BLTN_CAM_Time_Stm_NvalueSet = arr_random_setting[13];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);   
    _HU_BLTN_CAM_E_00.BLTN_CAM_TrnSig_Stm_NvalueSet = arr_random_setting[14];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);   
    _HU_BLTN_CAM_E_00.BLTN_CAM_AudRec_NvalueSet = arr_random_setting[15];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);   
    // ETC Settings
    _HU_BLTN_CAM_E_01.BLTN_CAM_FR_VerPos_NvalueSet = arr_random_setting[16];
    output(_HU_BLTN_CAM_E_01);
    testWaitForTimeout(200);   
    _HU_BLTN_CAM_E_01.BLTN_CAM_RR_VerPos_NvalueSet = arr_random_setting[17];
    output(_HU_BLTN_CAM_E_01);
    testWaitForTimeout(200);     
    _HU_BLTN_CAM_E_00.BLTN_CAM_Sen_DEV_NvalueSet = arr_random_setting[18];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);    
    _HU_BLTN_CAM_E_00.BLTN_CAM_Sen_PEV_NvalueSet = arr_random_setting[19];
    output(_HU_BLTN_CAM_E_00);
    testWaitForTimeout(200);    
    //App Notification
    if ((arr_random_setting[20] == 0x2 && getSignal(BLTN_CAM_Set_PEV_AppNoti) == 0x1) || (arr_random_setting[20] == 0x1 && getSignal(BLTN_CAM_Set_PEV_AppNoti) == 0x2)){
      @panel::Button_AppNoti_PEV = 1;
    }
  }
  
  
int checkBatteryType(char a[], int size)
{
  int i, matchPLBM, matchEAGM;
  
  matchPLBM = 1;
  
  matchEAGM = 1;
  
  for (i = 0; i < size; i++)
  {
    if (a[i] != PLBM_Battery[i])
    {
      matchPLBM = 0;
    }
    if (a[i] != eAGM_Battery[i])
    {
      matchEAGM = 0;
    }
  }

  if (matchPLBM)
    return 1;
  else if (matchEAGM)
    return 2;
  else
    return 0;
}

void Set_Battery(){
  int flag_Timeout; 
  int battery_Type; 
  
  flag_Timeout = 0;
  battery_Type = 0;
  
  @panel::Switch_CGWNM = 1;
  
  KEY_ON();
  
  do
  {
    testWaitForStringInput("Please enter kind of battery (PLBM or EAGM):", 300000);
    
    testGetStringInput(answer, 5);
    
    write("You entered: %c%c%c%c", answer[0], answer[1], answer[2], answer[3]);
    
    battery_Type = checkBatteryType(answer,4);
    
    if(battery_Type == 0){
      write("Invalid input! Please enter 'PLBM' or 'EAGM'.");
    }
  } while(battery_Type == 0);
  
  switch(battery_Type){
    case(1): 
      write("Battery type is PLBM.");     
      @ControlPowerSupply::RadioBtn_PwrSplyModel = 1; //Choose model power suplly TL305TP
      @ControlPowerSupply::Numeric_PortNum = 3; //Choose numberic port to connect PC 
 
      if( @ControlPowerSupply::RadioBtn_PwrSplyModel == 1 && @ControlPowerSupply::Numeric_PortNum == 3 ){
        while (@ControlPowerSupply::isConectedPwrSply == 0 && flag_Timeout <= 30){
        @ControlPowerSupply::Button_Connect = 0; 
        testWaitForTimeout(1000);
        @ControlPowerSupply::Button_Connect = 1;
        testWaitForTimeout(1000);  
        @ControlPowerSupply::Button_Connect = 0;
        flag_Timeout++;  
        }
      }
      flag_Timeout = 0;
      break;
    case(2):
      write("Battery type is EAGM.");
      setSignal(ICU_BS2SoC, 0x50); break;
  }

  KEY_OFF();
  
  @panel::Switch_CGWNM = 0;
}
  
  void Get_setting(){
    arr_get_setting[0] = getSignal(BLTN_CAM_RecSet_OWD);
    arr_get_setting[1] = getSignal(BLTN_CAM_RecSet_OWP);
    arr_get_setting[2] = getSignal(BLTN_CAM_RecSet_DEV);
    arr_get_setting[3] = getSignal(BLTN_CAM_RecSet_PEV);
    arr_get_setting[4] = getSignal(BLTN_CAM_Set_EV_RecMaxTime);
    arr_get_setting[5] = getSignal(BLTN_CAM_Set_PrkTime);
    arr_get_setting[6] = getSignal(BLTN_CAM_Set_Drv_TML_Int);
    arr_get_setting[7] = getSignal(BLTN_CAM_Set_EV_BfrTime);
    arr_get_setting[8] = getSignal(BLTN_CAM_Set_EV_AftTime);
    arr_get_setting[9] = getSignal(BLTN_CAM_Set_EVSen_Stm);
    arr_get_setting[10] = getSignal(BLTN_CAM_Set_GearPos_Stm);
    arr_get_setting[11] = getSignal(BLTN_CAM_Set_Locatn_Stm);
    arr_get_setting[12] = getSignal(BLTN_CAM_Set_Spd_Stm);
    arr_get_setting[13] = getSignal(BLTN_CAM_Set_Tim_Stm);
    arr_get_setting[14] = getSignal(BLTN_CAM_Set_TrnSig_Stm);
    arr_get_setting[15] = getSignal(BLTN_CAM_AudRec_Set);
    arr_get_setting[16] = getSignal(BLTN_CAM_Set_FR_Ver_Pos);
    arr_get_setting[17] = getSignal(BLTN_CAM_Set_RR_Ver_Pos);
    arr_get_setting[18] = getSignal(BLTN_CAM_SenSet_DEV);
    arr_get_setting[19] = getSignal(BLTN_CAM_SenSet_PEV);
    arr_get_setting[20] = getSignal(BLTN_CAM_Set_PEV_AppNoti);
  }
  
  void Random_Settings(){
    for(i = 0; i <= 20; i++){
     if ((i <= 3) || (i >= 9 && i <= 15) || (i == 20)){
       index = random(2);
       arr_random_setting[i] = arr_RecSet[index];
     }
     else if (i == 4){
       arr_random_setting[4] = set_120h_value;
     }
     else if (i == 5){
       index = random(19);
       arr_random_setting[5] = arr_OWPTime[index];
     }
     else if (i == 6){
       index = random(3);
       arr_random_setting[6] = arr_TMLinterval[index];
     }
     else if (i == 7 || i == 8){
       index = random(3);
       arr_random_setting[i] = arr_EVtime[index];
     }
     else if (i == 16 || i == 17){
       index = random(10);
       arr_random_setting[i] = arr_CAMAngle[index];
     }
     else{
       index = random(5);
       arr_random_setting[i] = arr_EVSen[index];
     }
    }
  }
  
  void Set_Profile_Guest(){
    int same;
    int loop;
    
    loop = 0;
  
    setSignal(IAU_ProfileIDRVal, 0x1); //Guest's Profile 
    
    testWaitForTimeout(5000);
    
    Random_Settings();
    write("=== RANDOM SETTING ===");
    for (i = 0; i <= 20; i++){
     write("arr_random_setting[%d] = 0x%X", i, arr_random_setting[i]);
    }
  
    do {
      Enter_BLTN_CAM();
      testWaitForTimeout(3000);
      
      BLTN_CAM_Settings();
      
      Exit_BLTN_CAM();
      testWaitForTimeout(3000);
      Get_setting();
      
      same = 1;
      for (i = 0; i <= 20; i++) {
        if (arr_random_setting[i] != arr_get_setting[i]) {
          same = 0;
          switch(i){
            case(0): write("Mismatch >> OWD: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(1): write("Mismatch >> OWP: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(2): write("Mismatch >> DEV: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(3): write("Mismatch >> PEV: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(4): write("Mismatch >> 120h: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(5): write("Mismatch >> Parking Time: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(6): write("Mismatch >> TML Interval: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(7): write("Mismatch >> Bfr Time: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(8): write("Mismatch >> Aft Time: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(9): write("Mismatch >> EV Sen Stamp: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(10): write("Mismatch >> Gear Stamp: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(11): write("Mismatch >> Location Stamp: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(12): write("Mismatch >> Velocity Stamp: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(13): write("Mismatch >> Time Stamp: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(14): write("Mismatch >> Turn Signal Stamp: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(15): write("Mismatch >> Audio: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(16): write("Mismatch >> FCAM Angle: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(17): write("Mismatch >> RCAM Angle: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(18): write("Mismatch >> DEV Sen Level: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(19): write("Mismatch >> PEV Sen Level: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(20): write("Mismatch >> App Notification: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
          }
          break;
        }
      }
      loop++;
      
      if (loop >= 2){
        switch(i){
          case(0): @panel::LED_Ctrl_SetOWP = 1; break;
          case(1): @panel::LED_Ctrl_SetOWD = 1; break;
          case(2): @panel::LED_Ctrl_SetPEV = 1; break;
          case(3): @panel::LED_Ctrl_SetDEV = 1; break;
          case(5): @panel::TrackBar_OWP_TIME = (int)arr_random_setting[5]; break; 
          case(6): @panel::TrackBar_Timelaps_TIME = (int)arr_random_setting[6]; break;
          case(7): if (arr_random_setting[7] == 0x0){
                     @panel::TrackBar_EV_Time_Bfr = 1;
                   }
                   else if (arr_random_setting[7] == 0x5){
                     @panel::TrackBar_EV_Time_Bfr = 2;
                   }
                   else {
                     @panel::TrackBar_EV_Time_Bfr = 3;                   
                   }
                   break;
          case(8): @panel::TrackBar_EV_Time_Aft = arr_random_setting[8] + 10;
          case(9): @panel::LED_Ctrl_Set_EVSen_Stm = 1; break;  
          case(10): @panel::LED_Ctrl_Set_GearPos_Stm = 1; break;
          case(11): @panel::LED_Ctrl_Set_Locatn_Stm = 1; break;
          case(12): @panel::LED_Ctrl_Set_Spd_Stm = 1; break;           
          case(13): @panel::LED_Ctrl_Set_Tim_Stm = 1; break; 
          case(14): @panel::LED_Ctrl_Set_TrnSig_Stm = 1; break; 
          case(15): @panel::LED_Ctrl_AudRec_Set = 1; break; 
          case(16): @panel::TrackBar_FR_Ver_Pos = (int)arr_random_setting[16]; break; 
          case(17): @panel::TrackBar_RR_Ver_Pos = (int)arr_random_setting[17]; break;         
          case(18): @panel::TrackBar_DEV_Sen = (int)arr_random_setting[19]; break; 
          case(19): @panel::TrackBar_PEV_Sen = (int)arr_random_setting[19]; break; 
          case(20): @panel::Button_AppNoti_PEV = 1; break;      
        }
      } 
    } while (!same);
    
    loop = 0;
    
    write("=== PROFILE GUEST ===");  
    
    for (i = 0; i <= 20; i++){
     arr_Profile_Guest_setting[i] = arr_get_setting[i];
     write("Profile_Guest_setting[%d] = 0x%X", i, arr_Profile_Guest_setting[i]);
    }
    
  }
  
  void Set_Profile_Guest_17(){
    int same;
    int loop;
    
    loop = 0;
  
    Profile_value = 0x1; //Guest's Profile 
    
    testWaitForTimeout(5000);
    
    Random_Settings();
    write("=== RANDOM SETTING ===");
    for (i = 0; i <= 20; i++){
     write("arr_random_setting[%d] = 0x%X", i, arr_random_setting[i]);
    }
  
    do {
      Enter_BLTN_CAM();
      testWaitForTimeout(3000);
      
      BLTN_CAM_Settings();
      
      Exit_BLTN_CAM();
      testWaitForTimeout(3000);
      Get_setting();
      
      same = 1;
      for (i = 0; i <= 20; i++) {
        if (arr_random_setting[i] != arr_get_setting[i]) {
          same = 0;
          switch(i){
            case(0): write("Mismatch >> OWD: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(1): write("Mismatch >> OWP: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(2): write("Mismatch >> DEV: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(3): write("Mismatch >> PEV: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(4): write("Mismatch >> 120h: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(5): write("Mismatch >> Parking Time: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(6): write("Mismatch >> TML Interval: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(7): write("Mismatch >> Bfr Time: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(8): write("Mismatch >> Aft Time: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(9): write("Mismatch >> EV Sen Stamp: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(10): write("Mismatch >> Gear Stamp: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(11): write("Mismatch >> Location Stamp: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(12): write("Mismatch >> Velocity Stamp: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(13): write("Mismatch >> Time Stamp: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(14): write("Mismatch >> Turn Signal Stamp: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(15): write("Mismatch >> Audio: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(16): write("Mismatch >> FCAM Angle: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(17): write("Mismatch >> RCAM Angle: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(18): write("Mismatch >> DEV Sen Level: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(19): write("Mismatch >> PEV Sen Level: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
            case(20): write("Mismatch >> App Notification: random = 0x%X, get = 0x%X", arr_random_setting[i], arr_get_setting[i]); break;
          }
          break;
        }
      }
      loop++;
      
      if (loop >= 2){
        switch(i){
          case(0): @panel::LED_Ctrl_SetOWP = 1; break;
          case(1): @panel::LED_Ctrl_SetOWD = 1; break;
          case(2): @panel::LED_Ctrl_SetPEV = 1; break;
          case(3): @panel::LED_Ctrl_SetDEV = 1; break;
          case(5): @panel::TrackBar_OWP_TIME = (int)arr_random_setting[5]; break; 
          case(6): @panel::TrackBar_Timelaps_TIME = (int)arr_random_setting[6]; break;
          case(7): if (arr_random_setting[7] == 0x0){
                     @panel::TrackBar_EV_Time_Bfr = 1;
                   }
                   else if (arr_random_setting[7] == 0x5){
                     @panel::TrackBar_EV_Time_Bfr = 2;
                   }
                   else {
                     @panel::TrackBar_EV_Time_Bfr = 3;                   
                   }
                   break;
          case(8): @panel::TrackBar_EV_Time_Aft = arr_random_setting[8] + 10;
          case(9): @panel::LED_Ctrl_Set_EVSen_Stm = 1; break;  
          case(10): @panel::LED_Ctrl_Set_GearPos_Stm = 1; break;
          case(11): @panel::LED_Ctrl_Set_Locatn_Stm = 1; break;
          case(12): @panel::LED_Ctrl_Set_Spd_Stm = 1; break;           
          case(13): @panel::LED_Ctrl_Set_Tim_Stm = 1; break; 
          case(14): @panel::LED_Ctrl_Set_TrnSig_Stm = 1; break; 
          case(15): @panel::LED_Ctrl_AudRec_Set = 1; break; 
          case(16): @panel::TrackBar_FR_Ver_Pos = (int)arr_random_setting[16]; break; 
          case(17): @panel::TrackBar_RR_Ver_Pos = (int)arr_random_setting[17]; break;         
          case(18): @panel::TrackBar_DEV_Sen = (int)arr_random_setting[19]; break; 
          case(19): @panel::TrackBar_PEV_Sen = (int)arr_random_setting[19]; break; 
          case(20): @panel::Button_AppNoti_PEV = 1; break;      
        }
      } 
    } while (!same);
    
    loop = 0;
    
    write("=== PROFILE GUEST ===");  
    
    for (i = 0; i <= 20; i++){
     arr_Profile_Guest_setting[i] = arr_get_setting[i];
     write("Profile_Guest_setting[%d] = 0x%X", i, arr_Profile_Guest_setting[i]);
    }
    
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
  
  void Create_DRV_Recorded_vid(){
    @panel::Switch_CGWNM = 1;
    
    write("KEY_ON"); KEY_ON();
    testWaitForTimeout(3000);
  
    do {
      Enter_BLTN_CAM();
      testWaitForTimeout(3000);
      if(getSignal(BLTN_CAM_RecSet_OWD) == 0x1){  
        @panel::LED_Ctrl_SetOWD = 1;
        write("OWD ON");
        testWaitForTimeout(500);
      }
      
      if(getSignal(BLTN_CAM_RecSet_DEV) == 0x1){   
        @panel::LED_Ctrl_SetDEV = 1;
        write("DEV ON");
        testWaitForTimeout(500);
      }
      
      flag_RecSet_error++;
        
      Exit_BLTN_CAM();
        
      } while ((getSignal(BLTN_CAM_RecSet_OWD) == 0x1 || getSignal(BLTN_CAM_RecSet_DEV) == 0x1) && (flag_RecSet_error < 15));
      
      flag_RecSet_error = 0;
      
    if (testWaitForSignalChange(BLTN_CAM_RecSta_OWD,10000) == 1){
     
      write("OWD recording is started");
      
      testWaitForTimeout(10000);
  
      openPanel("Note");
      
      if (testWaitForSignalChange(BLTN_CAM_RecSta_DEV,10000) == 1){
        write("DEV recording is started"); closePanel("Note");    
        
        @panel::Button_Timelaps_Stop = 1;
        testWaitForTimeout(500);
        @panel::Button_Timelaps_Start = 1;
      
        if (testWaitForSignalChange(BLTN_CAM_RecSta_TML,10000) == 1){
          write("TML recording is started");
        
          @panel::Button_SwitchEvt = 0;
          testWaitForTimeout(500);
          @panel::Button_SwitchEvt = 1;
        
          if (testWaitForSignalChange(BLTN_CAM_RecSta_MAR,10000) == 1){
            write("MAR recording is started");
            testWaitForTimeout(20000);
          }
        }
      }
      else {
      
        write("Error: OWD recording hasn't started");
      
        testStepWarning("Warning","OWD recording hasn't started");
      
        Set_All_Recording_OFF();
      
        stop();
      }
    }
  }
  
  void Create_PRK_Recorded_vid(){
    @panel::Switch_CGWNM = 1;
    
    write("KEY_ON"); KEY_ON();
    testWaitForTimeout(3000);
    
    setSignal(ICU_PowerAutoCutModSta,0x2);
  
    do {
      Enter_BLTN_CAM();
      testWaitForTimeout(3000);
      
      if(getSignal(BLTN_CAM_RecSet_OWP) == 0x1){      
        @panel::LED_Ctrl_SetOWP = 1;
        write("OWP ON");
        testWaitForTimeout(500);
      }
  
      if(getSignal(BLTN_CAM_RecSet_PEV) == 0x1){   
        @panel::LED_Ctrl_SetPEV = 1;
        write("PEV ON");
        testWaitForTimeout(500);
      }  
      flag_RecSet_error++;
        
      Exit_BLTN_CAM();
        
      } while ((getSignal(BLTN_CAM_RecSet_OWP) == 0x1 || getSignal(BLTN_CAM_RecSet_PEV) == 0x1) && (flag_RecSet_error < 15));
      
    flag_RecSet_error = 0;
    
    write("KEY_OFF"); KEY_OFF();  
    
    if (testWaitForSignalChange(BLTN_CAM_RecSta_OWP,10000) == 1){
      write("OWP recording is started");
      testWaitForTimeout(10000);
      
      openPanel("Note");
      
      if (testWaitForSignalChange(BLTN_CAM_RecSta_PEV,10000) == 1){
        write("PEV recording is started"); closePanel("Note");
        
        @panel::Button_SwitchEvt = 0;
        testWaitForTimeout(500);
        @panel::Button_SwitchEvt = 1;
        
        if (testWaitForSignalChange(BLTN_CAM_RecSta_MAR,10000) == 1){
          write("MAR recording is started");
          testWaitForTimeout(20000);
        }
      } 
    }
    else {
      
      write("Error: OWP recording hasn't started");
      
      testStepWarning("Warning","OWP recording hasn't started");
      
      Set_All_Recording_OFF();
      
      stop();
    }
  }

  void Create_DRV_Recorded_vid_17(){
    @panel::Switch_CGWNM = 1;
  
    KEY_state = 0x2;
    testWaitForTimeout(3000);
    
    do {
      Enter_BLTN_CAM();
      testWaitForTimeout(3000);
      if(getSignal(BLTN_CAM_RecSet_OWD) == 0x1){  
        @panel::LED_Ctrl_SetOWD = 1;
        write("OWD ON");
        testWaitForTimeout(500);
      }
      
      if(getSignal(BLTN_CAM_RecSet_DEV) == 0x1){   
        @panel::LED_Ctrl_SetDEV = 1;
        write("DEV ON");
        testWaitForTimeout(500);
      }
      
      flag_RecSet_error++;
        
      Exit_BLTN_CAM();
        
      } while ((getSignal(BLTN_CAM_RecSet_OWD) == 0x1 || getSignal(BLTN_CAM_RecSet_DEV) == 0x1) && (flag_RecSet_error < 15));
      
      flag_RecSet_error = 0;
      
    if (testWaitForSignalChange(BLTN_CAM_RecSta_OWD,10000) == 1){
     
      write("OWD recording is started");
      
      testWaitForTimeout(10000);
  
      openPanel("Note");
      
      if (testWaitForSignalChange(BLTN_CAM_RecSta_DEV,10000) == 1){
        write("DEV recording is started"); closePanel("Note");    
        
        @panel::Button_Timelaps_Stop = 1;
        testWaitForTimeout(500);
        @panel::Button_Timelaps_Start = 1;
      
        if (testWaitForSignalChange(BLTN_CAM_RecSta_TML,10000) == 1){
          write("TML recording is started");
        
          @panel::Button_SwitchEvt = 0;
          testWaitForTimeout(500);
          @panel::Button_SwitchEvt = 1;
        
          if (testWaitForSignalChange(BLTN_CAM_RecSta_MAR,10000) == 1){
            write("MAR recording is started");
            testWaitForTimeout(20000);
          }
        }
      }
      else {
      
        write("Error: OWD recording hasn't started");
      
        testStepWarning("Warning","OWD recording hasn't started");
      
        Set_All_Recording_OFF();
      
        stop();
      }
    }
  }

  void Create_PRK_Recorded_vid_17(){
    @panel::Switch_CGWNM = 1;
    
    KEY_state = 0x2; 
    testWaitForTimeout(3000);
    
    Mode_state = 0x2; 
    
    do {
      Enter_BLTN_CAM();
      testWaitForTimeout(3000);
      
      if(getSignal(BLTN_CAM_RecSet_OWP) == 0x1){      
        @panel::LED_Ctrl_SetOWP = 1;
        write("OWP ON");
        testWaitForTimeout(500);
      }
  
      if(getSignal(BLTN_CAM_RecSet_PEV) == 0x1){   
        @panel::LED_Ctrl_SetPEV = 1;
        write("PEV ON");
        testWaitForTimeout(500);
      }  
      flag_RecSet_error++;
        
      Exit_BLTN_CAM();
        
      } while ((getSignal(BLTN_CAM_RecSet_OWP) == 0x1 || getSignal(BLTN_CAM_RecSet_PEV) == 0x1) && (flag_RecSet_error < 15));
      
    flag_RecSet_error = 0;
    
    KEY_state = 0x0;  
    
    if (testWaitForSignalChange(BLTN_CAM_RecSta_OWP,10000) == 1){
      write("OWP recording is started");
      testWaitForTimeout(10000);
      
      openPanel("Note");
      
      if (testWaitForSignalChange(BLTN_CAM_RecSta_PEV,10000) == 1){
        write("PEV recording is started"); closePanel("Note");
        
        @panel::Button_SwitchEvt = 0;
        testWaitForTimeout(500);
        @panel::Button_SwitchEvt = 1;
        
        if (testWaitForSignalChange(BLTN_CAM_RecSta_MAR,10000) == 1){
          write("MAR recording is started");
          testWaitForTimeout(20000);
        }
      } 
    }
    else {
      
      write("Error: OWP recording hasn't started");
      
      testStepWarning("Warning","OWP recording hasn't started");
      
      Set_All_Recording_OFF();
      
      stop();
    }
  }

  void Init() {

    condition1 = 0;
    condition2 = 0;
    condition3 = 0;

  }

  void Init_17() {

    flag = 0;
    flag_RecSet_error = 0;
    condition1 = 0; condition2 = 0; condition3 = 0;
    KEY_state = 0x0;
    Profile_value = 0x1;
    Mode_state = 0x2;
    Bat_val = 0x50;
    
    testWaitForTimeout(1000);
  
    send_IAU_ProfileIDRVal_timer.cancel();
    send_SMK_TrmnlCtrlGrpStaBDC_timer.cancel();
    send_ICU_PowerAutoCutModSta_timer.cancel();
    send_ICU_BS2SoC_timer.cancel();

  }  
