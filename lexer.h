#include<stdio.h>
#ifndef LEXER_H
#define LEXER_H

#define MAX_KEYWORDS            20
#define MAX_STANDARD_WORDS      13
#define MAXIMUM_TOKEN_SIZE      1000

typedef enum{
    KEYWORD,
    OPERATOR,
    SPECIAL_CHARACTER,
    CONSTANT,
    IDENTIFIER,
    LITERAL,
    PREPROCESSOR,
    COMMENTED_LINE,
    UNKNOWN
}Tokentype;

typedef struct{
    char lexeme[MAXIMUM_TOKEN_SIZE];
    Tokentype type;
}Token;

FILE*initiatelexer(const char *name_of_file);
Token checknextToken(FILE*fptr,int size,Token prevtok);
Token group_various_token(Token tokenvar,FILE * fptr,int size,Token prevtok);
int isKeyword(const char*str);
int isOperator(const char*str);
int isSpecialCharacter(const char*str);
int isConstant(const char*str);
int isliteral(const char *str);
int ispreprocessor(const char*str);
int iscomment(const char*str);
int checkendparenthesis(FILE*fptr, int size);
int checkendof_singleinvertedcomma(FILE*fptr,int size);
int checkendof_invertedcomma(FILE*fptr, int size);
int checkendsquarebracket(FILE*fptr,int size);
int check_end_of_curlybraces(FILE*fptr,int size);
int checksemicolon(const char*str, FILE*fptr, int size, Token prevtok);
int linecount(FILE*fptr, int limit);
Token group_various_token(Token tokenvar,FILE * fptr,int size,Token prevtok);

#endif