/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include "reader.h"

FILE *inputStream;
int lineNo, colNo;
int currentChar;

void toLowerCase(char *str)
{
  for (int i = 0; i <= strlen(str); i++)
  {
    if (str[i] >= 65 && str[i] <= 90)
      str[i] = str[i] + 32;
  }
}

void toLowerCaseLetter() {
      if (currentChar >= 65 && currentChar <= 90)
      currentChar = currentChar + 32;
}

int readChar(void) {
  currentChar = getc(inputStream);
  toLowerCaseLetter();
  colNo ++;
  if (currentChar == '\n') {
    lineNo ++;
    colNo = 0;
  }
  return currentChar;
}

int openInputStream(char *fileName) {
  inputStream = fopen(fileName, "rt");
  if (inputStream == NULL)
    return IO_ERROR;
  lineNo = 1;
  colNo = 0;
  readChar();
  return IO_SUCCESS;
}

void closeInputStream() {
  fclose(inputStream);
}

