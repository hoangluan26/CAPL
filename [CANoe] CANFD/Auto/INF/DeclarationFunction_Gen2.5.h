/*@!Encoding:1252*/
variables {
  msTimer send_BDC_FD_SMK_03_200ms_timer;
  message BDC_FD_SMK_03_200ms _BDC_FD_SMK_03_200ms;
  byte KEY_STATE;
}
//Declaration Function
on timer send_BDC_FD_SMK_03_200ms_timer {
  _BDC_FD_SMK_03_200ms.SMK_TrmnlCtrlGrpStaBDC = KEY_STATE;
  output(_BDC_FD_SMK_03_200ms);
  send_BDC_FD_SMK_03_200ms_timer.set(200);
}

void CaptureGraphics_Lft(){
  testWaitForTimeout(3000);
  testReportAddWindowCapture("INF","BLTN_CAM_RecSta_OWD; Lamp_TrnSigLmpLftOnReq","Sreenshot","./Husign.jpg");
}

void CaptureGraphics_Rt(){
  testWaitForTimeout(3000);
  testReportAddWindowCapture("INF","BLTN_CAM_RecSta_OWD; Lamp_TrnSigLmpRtOnReq","Sreenshot","./Husign.jpg");
}

void Enter_BLTN_CAM(){
//   CAN FD
  _HU_BLTN_CAM_02_200ms.HU_BLTN_CAM_UI_Mode = 0x1; //Enter BLTN_CAM mode
  output(_HU_BLTN_CAM_02_200ms);
}

void Exit_BLTN_CAM(){
//   CAN FD 
  _HU_BLTN_CAM_02_200ms.HU_BLTN_CAM_UI_Mode = 0x0; //Enter AVN mode - Exit BLTN_CAM Mode
  output(_HU_BLTN_CAM_02_200ms);
}

void Init(){
  
  @panel::Switch_CGWNM = 1; 
  
  KEY_STATE = 0x1; testWaitForTimeout(3000);
  
  if ($BLTN_CAM_RecSet_OWD == 0x2 || $BLTN_CAM_RecSet_OWP == 0x2 || $BLTN_CAM_RecSet_DEV == 0x2 || $BLTN_CAM_RecSet_PEV == 0x2){

    do {
      Enter_BLTN_CAM();      
      testWaitForTimeout(3000);
      if ($BLTN_CAM_RecSet_OWD == 0x2) {write("OWD OFF"); @panel::LED_Ctrl_SetOWD = 1;testWaitForTimeout(500);}
      else if ($BLTN_CAM_RecSet_OWP == 0x2) {write("OWP OFF"); @panel::LED_Ctrl_SetOWP = 1;testWaitForTimeout(500);}
      else if ($BLTN_CAM_RecSet_DEV == 0x2) {write("DEV OFF"); @panel::LED_Ctrl_SetDEV = 1;testWaitForTimeout(500);}
      else if ($BLTN_CAM_RecSet_PEV == 0x2) {write("PEV OFF"); @panel::LED_Ctrl_SetPEV = 1;testWaitForTimeout(500);}
      Exit_BLTN_CAM();
      testWaitForTimeout(2000);
      flag_RecSet_error++;
    } while($BLTN_CAM_RecSet_OWD == 0x2 || $BLTN_CAM_RecSet_OWP == 0x2 || $BLTN_CAM_RecSet_DEV == 0x2 || $BLTN_CAM_RecSet_PEV == 0x2 && (flag_RecSet_error < 15));
    
    flag_RecSet_error = 0; 

  }
  else {
    write("Notifications: All recording setting are turned OFF.");
  }

  while (getSignal(BLTN_CAM_SD_Format_Op_State) != 0x1){
    write("Reset Recorded Video"); @panel::Button_RecReset = 1; 
    testWaitForSignalChange(BLTN_CAM_SD_Format_Op_State,500);
  }
  
  
  KEY_STATE = 0x0;
  @panel::Switch_CGWNM = 0; 
}

void BLTN_CAM_Settings() {
  
  Enter_BLTN_CAM(); testWaitForTimeout(3000);

  if ($BLTN_CAM_RecSet_OWP == 0x1) {write("OWP On"); @panel::LED_Ctrl_SetOWP = 1; testWaitForTimeout(1000);}   
  
  Exit_BLTN_CAM();
  
}

// Check lo?i xe
int check_Model_Type(char a[], int size) {
  int len;
  len = my_strlen(a);

  if (len == 3 && a[0] == 'I' && a[1] == 'C' && a[2] == 'E') return 1;
  if (len == 3 && a[0] == 'H' && a[1] == 'E' && a[2] == 'V') return 2;
  if (len == 2 && a[0] == 'E' && a[1] == 'V') return 3;

  return 0;
}

/* helper: clear chu?i */
void clearString(char arr[], int size) {
  int i;
  for (i = 0; i < size; i++) arr[i] = '\0';
}

/* helper: tr? v? d? dài chu?i (simple strlen) */
int my_strlen(char s[]) {
  int i = 0;
  while (s[i] != '\0') i++;
  return i;
}


void Choose_ModelType() {
  int mode_type = 0;

  do {
    // clear tru?c khi d?c (an toàn)
    clearString(answer, 4);

    testWaitForStringInput("Enter model type car (ICE/HEV/EV)", 300000);
    testGetStringInput(answer, 4);    // d?c t?i da 3 ký t? (c?ng '\0')
    
    write("Your input: '%s'", answer);

    mode_type = check_Model_Type(answer, 4);

    if (mode_type == 0) {
      write("Invalid input! Please enter ICE or HEV or EV");
    }

    // clear tru?c khi l?p l?i (không b?t bu?c n?a nhung an toàn)
    clearString(answer, 4);

  } while (mode_type == 0);
  
  switch(mode_type){
    case(1):
      send_TCU_01_10ms_timer.set(10); break;
    case(2): 
      send_HTCU_04_10ms_timer.set(10); break;
    case(3):
      send_VCU_01_10ms_timer.set(10); break;
  }
}

void Set_Off_Stamp() {
  if ($BLTN_CAM_Set_EVSen_Stm == 0x2) {write(" Set EVSen Stamp Off"); @panel::LED_Ctrl_Set_EVSen_Stm = 1; testWaitForTimeout(500);};
  if ($BLTN_CAM_Set_GearPos_Stm == 0x2) {write("Set GearPos Stamp Off"); @panel::LED_Ctrl_Set_GearPos_Stm = 1; testWaitForTimeout(500);}
  if ($BLTN_CAM_Set_Locatn_Stm == 0x2) {write("Set Locatn Stamp Off"); @panel::LED_Ctrl_Set_Locatn_Stm = 1; testWaitForTimeout(500);}
  if ($BLTN_CAM_Set_Spd_Stm == 0x2) {write("Set Speed Stamp Off"); @panel::LED_Ctrl_Set_Spd_Stm = 1; testWaitForTimeout(500);}  
  if ($BLTN_CAM_Set_Tim_Stm == 0x2) {write("Set Time Stamp Off"); @panel::LED_Ctrl_Set_Tim_Stm = 1; testWaitForTimeout(500);}
  if ($BLTN_CAM_Set_TrnSig_Stm == 0x2) {write("Set TrnSig Stamp Off"); @panel::LED_Ctrl_Set_TrnSig_Stm = 1; testWaitForTimeout(500);}
}

void Choose_CarModel() {
   int mode_type = 0;

  do {
    // clear tru?c khi d?c (an toàn)
    clearString(answer, 4);

    testWaitForStringInput("Enter model type car (ICE/HEV/EV)", 300000);
    testGetStringInput(answer, 4);    // d?c t?i da 3 ký t? (c?ng '\0')
    
    write("Your input: '%s'", answer);

    mode_type = check_Model_Type(answer, 4);

    if (mode_type == 0) {
      write("Invalid input! Please enter ICE or HEV or EV");
    }

    // clear tru?c khi l?p l?i (không b?t bu?c n?a nhung an toàn)
    clearString(answer, 4);

  } while (mode_type == 0);
  
  switch(mode_type){
    case(1):
      @panel::RadioBtn_Vehicle_Type = 0; break;
    case(2): 
      @panel::RadioBtn_Vehicle_Type = 1; break;
    case(3):
      @panel::RadioBtn_Vehicle_Type = 2; break;
  }

  @panel::EnableDrvInfo = 1;
}

