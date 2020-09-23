/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"

extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

typedef struct {
  char letter[15];
  int linesCo;
  int lines[100];
} TokenTrack;
TokenTrack tokenTracks[500];

Token tokens[500]; 
int countToken = 0;


/***************************************************************/

void skipBlank()
{
  while (charCodes[currentChar] == CHAR_SPACE)
  {
    readChar();
  }
}

void skipComment()
{
  int isEnd = 0;
  while (1)
  {
    readChar();
    switch (charCodes[currentChar])
    {
    case CHAR_TIMES:
      readChar();
      if (charCodes[currentChar] == CHAR_RPAR)
      {
        isEnd = 1;
        readChar();
        break;
      }
    default:
      if (currentChar == EOF && isEnd == 0)
      {
        error(ERR_ENDOFCOMMENT, lineNo, colNo);
      }
    }
    if (isEnd == 1)
    {
      break;
    }
  };
}

Token *readIdentKeyword(void)
{
  Token *token;
  int ln = lineNo;
  int cn = colNo;
  char key[MAX_IDENT_LEN];
  key[0] = currentChar;
  int count = 1;
  while (count < MAX_IDENT_LEN)
  {
    key[count] = readChar();
    if (charCodes[key[count]] != CHAR_LETTER && charCodes[key[count]] != CHAR_DIGIT)
    {
      break;
    }
    count++;
  }
  key[count] = '\0';
  if (count == MAX_IDENT_LEN)
    error(ERR_IDENTTOOLONG, lineNo, colNo);
  else
  {
    TokenType tokenType = checkKeyword(key);
    if (tokenType != TK_NONE)
    {
      return makeToken(tokenType, ln, cn);
    }
    else
    {
      token = makeToken(TK_IDENT, ln, cn);
      strcpy(token->string, key);
      return token;
    }
  }
}

Token *readNumber(void)
{
  Token *token;
  int ln = lineNo;
  int cn = colNo;
  char key[11];
  key[0] = currentChar;
  int count = 1;
  readChar();
  while (charCodes[currentChar] == CHAR_DIGIT)
  {
    readChar();
    key[count] = currentChar;
    count++;
  }
  key[count] = '\0';
  token = makeToken(TK_NUMBER, ln, cn);
  token->value = atoi(key);
  strcpy(token->string, key);
  return token;
}

Token *readConstChar(void)
{
  int ln = lineNo;
  int cn = colNo;
  Token *token;
  char key[256];
  int count = 0;
  do
  {
    readChar();
    if (currentChar == EOF)
    {
      error(ERR_INVALIDCHARCONSTANT, ln, cn);
      break;
    }
    if(charCodes[currentChar] == CHAR_SINGLEQUOTE) break;
    key[count] = currentChar;
    count++;
  } while (charCodes[currentChar] != CHAR_SINGLEQUOTE);
  key[count] = '\0';
  token = makeToken(TK_CHAR, ln, cn);
  strcpy(token->string, key);
  readChar();
  return token;
}

Token *getToken(void)
{
  Token *token;
  int ln, cn;

  if (currentChar == EOF)
  {
    return makeToken(TK_EOF, lineNo, colNo);
  }

  switch (charCodes[currentChar])
  {
  case CHAR_SPACE:
    skipBlank();
    return getToken();

  case CHAR_LETTER:
    return readIdentKeyword();

  case CHAR_DIGIT:
    return readNumber();

  case CHAR_PLUS:
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar();
    return token;

  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return token;

  case CHAR_TIMES:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return makeToken(SB_TIMES, ln, cn);

  case CHAR_SLASH:
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar();
    return token;

  case CHAR_COMMA:
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;

  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;

  case CHAR_PERIOD:
    token = makeToken(SB_PERIOD, lineNo, colNo);
    readChar();
    return token;

  case CHAR_EQ:
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;

  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token;

  case CHAR_SINGLEQUOTE:

    return readConstChar();

  //cái kép: := , >= , <= , != , mo ngoac ,
  // :=
  case CHAR_COLON:
    ln = lineNo; // luu lai diem bat dau cua operator
    cn = colNo;
    readChar();
    if ((currentChar != EOF) &&
        (charCodes[currentChar] == CHAR_EQ))
    {
      readChar();
      return makeToken(SB_ASSIGN, ln, cn);
    }
    else
      return makeToken(SB_COLON, ln, cn);
  // >=
  case CHAR_GT:
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) &&
        (charCodes[currentChar] == CHAR_EQ))
    {
      readChar();
      return makeToken(SB_GE, ln, cn);
    }
    else
      return makeToken(SB_GT, ln, cn);
    // <=
  case CHAR_LT:
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) &&
        (charCodes[currentChar] == CHAR_EQ))
    {
      readChar();
      return makeToken(SB_LE, ln, cn);
    }
    else
      return makeToken(SB_LT, ln, cn);
    // !=
  case CHAR_EXCLAIMATION:
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) &&
        (charCodes[currentChar] == CHAR_EQ))
    {
      readChar();
      return makeToken(SB_NEQ, ln, cn);
    }
    else
      error(ERR_INVALIDSYMBOL, ln, cn);
    return makeToken(TK_NONE, ln, cn);

  // cac dau ngoac: (*  (.)
  case CHAR_LPAR:
    ln = lineNo;
    cn = colNo;
    readChar();
    if (currentChar == EOF)
    {
      error(ERR_INVALIDSYMBOL, ln, cn);
      return makeToken(TK_EOF, ln, cn);
    }

    switch (charCodes[currentChar])
    {
    case CHAR_PERIOD:
      readChar();
      return makeToken(SB_LSEL, ln, cn);

    case CHAR_TIMES:
      readChar();
      skipComment();
      return getToken();

    default:
      return makeToken(SB_LPAR, ln, cn);
    }
  }
}

/******************************************************************/

void trackToken(Token *token) {
  int isDup = isDuplicateToken(token);
    // printf("%s %d\n", token->string, countToken);

  if (isDup == -1) {
    strcpy(tokenTracks[countToken].letter, token->string);
    tokenTracks[countToken].linesCo = 0;
    tokenTracks[countToken].lines[0] = lineNo;
    countToken++;
  } else {
    tokenTracks[isDup].linesCo++;
    tokenTracks[isDup].lines[tokenTracks[countToken].linesCo] = lineNo;
  }
  // printf("%s %d %d\n", tokenTracks[3].letter, token->lineNo, token->colNo);
}

void printTokenTrack() {

  for (int i = 0 ; i<countToken; i++) {
    for (int j = 0; j<tokenTracks[i].linesCo; j++) {
      printf("%d, ", tokenTracks[i].lines[j]);
    }
    printf("%s\n", tokenTracks[i].letter);
  }
}

int isDuplicateToken(Token *token) {
  int isDuplicate = -1;
  for (int i = 0; i<countToken; i++) {
    if (!strcmp(token->string, tokenTracks[i].letter)) {
      return i;
    }
  }
  return isDuplicate;
}

void printLetter(Token *token) {
  if (isDuplicate(token) == 0) {
    tokens[countToken] = *token;
    countToken++;
    printf("%d - %d: ", token->lineNo, token->colNo);
    printf("%s\n", token->string);
  } else {
    
  }

}
// ------------------------------------------------------------------
int isDuplicate(Token *token) {
  int isDuplicate = 0;
  for (int i = 0; i<countToken; i++) {
    if (!strcmp(token->string, tokens[i].string)) {
      return 1;
    }
  }
  return isDuplicate;
}

void printToken(Token *token)
{

  printf("line: %d- col: %d  \n", token->lineNo, token->colNo);

  switch (token->tokenType)
  {
  case TK_NONE:
    printf("TK_NONE\n");
    break;
  case TK_IDENT:
    printf("TK_IDENT(%s)\n", token->string);
    break;
  case TK_NUMBER:
    printf("TK_NUMBER(%s)\n", token->string);
    break;
  case TK_CHAR:
    printf("TK_CHAR(\'%s\')\n", token->string);
    break;
  case TK_EOF:
    printf("TK_EOF\n");
    break;

  case KW_PROGRAM:
    printf("KW_PROGRAM\n");
    break;
  case KW_CONST:
    printf("KW_CONST\n");
    break;
  case KW_TYPE:
    printf("KW_TYPE\n");
    break;
  case KW_VAR:
    printf("KW_VAR\n");
    break;
  case KW_INTEGER:
    printf("KW_INTEGER\n");
    break;
  case KW_CHAR:
    printf("KW_CHAR\n");
    break;
  case KW_ARRAY:
    printf("KW_ARRAY\n");
    break;
  case KW_OF:
    printf("KW_OF\n");
    break;
  case KW_FUNCTION:
    printf("KW_FUNCTION\n");
    break;
  case KW_PROCEDURE:
    printf("KW_PROCEDURE\n");
    break;
  case KW_BEGIN:
    printf("KW_BEGIN\n");
    break;
  case KW_END:
    printf("KW_END\n");
    break;
  case KW_CALL:
    printf("KW_CALL\n");
    break;
  case KW_IF:
    printf("KW_IF\n");
    break;
  case KW_THEN:
    printf("KW_THEN\n");
    break;
  case KW_ELSE:
    printf("KW_ELSE\n");
    break;
  case KW_WHILE:
    printf("KW_WHILE\n");
    break;
  case KW_DO:
    printf("KW_DO\n");
    break;
  case KW_FOR:
    printf("KW_FOR\n");
    break;
  case KW_TO:
    printf("KW_TO\n");
    break;

  case SB_SEMICOLON:
    printf("SB_SEMICOLON\n");
    break;
  case SB_COLON:
    printf("SB_COLON\n");
    break;
  case SB_PERIOD:
    printf("SB_PERIOD\n");
    break;
  case SB_COMMA:
    printf("SB_COMMA\n");
    break;
  case SB_ASSIGN:
    printf("SB_ASSIGN\n");
    break;
  case SB_EQ:
    printf("SB_EQ\n");
    break;
  case SB_NEQ:
    printf("SB_NEQ\n");
    break;
  case SB_LT:
    printf("SB_LT\n");
    break;
  case SB_LE:
    printf("SB_LE\n");
    break;
  case SB_GT:
    printf("SB_GT\n");
    break;
  case SB_GE:
    printf("SB_GE\n");
    break;
  case SB_PLUS:
    printf("SB_PLUS\n");
    break;
  case SB_MINUS:
    printf("SB_MINUS\n");
    break;
  case SB_TIMES:
    printf("SB_TIMES\n");
    break;
  case SB_SLASH:
    printf("SB_SLASH\n");
    break;
  case SB_LPAR:
    printf("SB_LPAR\n");
    break;
  case SB_RPAR:
    printf("SB_RPAR\n");
    break;
  case SB_LSEL:
    printf("SB_LSEL\n");
    break;
  case SB_RSEL:
    printf("SB_RSEL\n");
    break;
  }
}

int scan(char *fileName)
{
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF)
  {
    // printToken(token);
    // printLetter(token);
    trackToken(token);
    free(token);
    token = getToken();
     
  }
  printTokenTrack();

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[])
{
  if (argc <= 1)
  {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR)
  {
    printf("Can\'t read input file!\n");
    return -1;
  }

  return 0;
}
