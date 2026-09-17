/*@!Encoding:949*/
includes
{
  
}

variables
{
  
}

/// <배열조작>
void string_PLUS_double(char inputString[], double inputValue, char outputString[])
{
  char STRING[50];
  char buffer[50];
  
  strncpy(outputString, buffer, 50); //버퍼 초기화
  strncpy(STRING, inputString, 50);
  
  snprintf(buffer, elcount(buffer), "%.1f", inputValue); //double to char
  //write("snprintf: %s, %d", buffer, strlen(buffer));

  strncpy(outputString, STRING, 50); //
  strncat(outputString, buffer, strlen(STRING) + strlen(buffer) + 1); //char + char
}

void byteToChar(byte byteArray[], char stringBuffer[])
{
  int i;

  // 바이트 데이터를 문자열로 변환
  for (i = 0; i < elcount(byteArray) ; i++)
    stringBuffer[i] = byteArray[i];
}
/// <배열조작>
