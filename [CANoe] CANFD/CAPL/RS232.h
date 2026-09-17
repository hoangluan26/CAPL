/*@!Encoding:949*/
includes
{
  #include "../CAPL/DataTransform.h"
}

variables
{
  //Rs 232 Send
  msTimer timer_RS232SendInterval;
  int TIME_RS232SendInterval = 100;

  int bufferIndex = 0;
  char buffer[3][50];
  char arrayInitVal[50]; //버퍼 초기화용  
  
  //Rs 232 Receive
  msTimer timer_RS232ReceiveInterval;
  int TIME_RS232ReceiveInterval = 1000;
  char receiverBuffer[50];
  
  enum Model_PowerSupply
  {
    OPE305QI = 0x0,
    TL305TP = 0x1,
    GPP2323 = 0x2
  };
}

/// <RS232 - Send Msg>
void Rs232SendStirng(int port, int PowerSupplyModel, char text[])
{
  byte buf[50];
  int i, j;
  int length;
  length=strlen(text);
  
  if(@ControlPowerSupply::isConectedPwrSply)
  {
    //버퍼 초기화
    for (i = 0; i < length ; i++) buf[i] = 0; 
    
    //char to byte
    for (i=0;i<length;i++) buf[i]=text[i];
    
    //개행문자 달기
    if((PowerSupplyModel == OPE305QI) || (PowerSupplyModel == GPP2323))
    {
      //Line Feed(개행문자)
      buf[length] = 0xA; 
      
      //for(j=0; j<length+1; j++) write("%d %c", j, buffer[j]);
      if ( 1==RS232Send(port, buf, length+1) ) write("Send: %s", text);
    }
    
    else if(PowerSupplyModel == TL305TP)
    {
      //개행문자 없음
      //for(j=0; j<length+1; j++) write("%d %c", j, buffer[j]);
      if ( 1==RS232Send(port, buf, length) ) write("Send: %s", text);
    }
  }
  else
  {
    write("The powersupply is not connected . Connect the powersupply.");
  }
}

/// <RS232 - Send Msg>
on timer timer_RS232SendInterval
{
  int i;
  
  // buffer[0] ~ [2] 까지 보내기
  if(bufferIndex < 3)
  {
    Rs232SendStirng(@ControlPowerSupply::Numeric_PortNum, OPE305QI, buffer[bufferIndex]);
    bufferIndex++;
    
    timer_RS232SendInterval.set(TIME_RS232SendInterval);
  }
  //다보냈으면 끝
  else
  {
    for(i=0; i<3; i++) strncpy(buffer[i], arrayInitVal, 50); //버퍼 초기화
    bufferIndex = 0;
    
    cancelTimer(timer_RS232SendInterval);
  }
}

/// <RS232 - Receive Msg>
void Rs232ReceiveMsg()
{
  timer_RS232ReceiveInterval.set(TIME_RS232ReceiveInterval);
}

/// <RS232 - Receive Msg>
on timer timer_RS232ReceiveInterval
{
  byte byteBuffer[10];
 
  rs232Receive(@ControlPowerSupply::Numeric_PortNum, byteBuffer, 10);
  byteToChar(byteBuffer, receiverBuffer);
  write("Received msg: %s", receiverBuffer);
  
  cancelTimer(timer_RS232ReceiveInterval);
}
