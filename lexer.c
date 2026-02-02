#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include <stdlib.h>
#include <unistd.h>

static const char*keywords[MAX_KEYWORDS]={
    "int","float","return","if","else","while","for","do","break","continue",
    "char","double","void","switch","case","default","const","static","sizeof","struct"
};

static const char*standard_keywords[MAX_STANDARD_WORDS]={
    "int","float","return","break","continue","char","double","void","const",
    "static","struct","printf","scanf"};

static const char*operators = "+-*/%=!<>|&";
static const char*specialcharacters = ",;{}()[]";

FILE*initiatelexer(const char *name_of_file){
    FILE*fptr;
    fptr = fopen(name_of_file, "r");
    if(fptr == NULL){
        fprintf(stderr,"ERROR : FILE OPENING UNSUCCESSFULL\n");
        return NULL;
    }
    printf("File %s is opening successfully\n",name_of_file);
    printf("Parsing\t: %s: Started\n",name_of_file);
    return fptr;
}

Token checknextToken(FILE*fptr,int size,Token prevtok){
    Token tokenvar;
    char ch, str[1000];
    fread(&ch,1,1,fptr);
    if(ch == EOF){
        printf("End of file is encountered\n");
        tokenvar.type=UNKNOWN;
        return tokenvar;       
    }
    while(ch == ' ' || ch == '\n' || ch == '\r'){
        fread(&ch,1,1,fptr);
        if(ftell(fptr) >= size){
            printf("Parsing : Done\n");
            exit(1);
        }
    }
    if(ch == '('){
        int sizef = ftell(fptr);
        if(checkendparenthesis(fptr,size) == 1){
            fseek(fptr,-1,SEEK_CUR);
        }
        else{
            printf("Paranthesis not ended\n");
            exit(1);
        }
        int sizel = ftell(fptr);
        int move = sizef - sizel;
        fseek(fptr,move,SEEK_CUR);
    }
    if(ch == '"'){
        int sizef = ftell(fptr);
        int flag=0;
        char chtemp = fgetc(fptr);
        fseek(fptr,-1,SEEK_CUR);
        if(chtemp == '\''){
            flag = 1;
        }
        if(flag == 0){
            if(checkendof_invertedcomma(fptr,size) == 1){
                fseek(fptr,-1,SEEK_CUR);
            }
            else{
                printf("ERROR: Double inverted comma is not ended properly\n");
                exit(1);
            }
        }
        int sizel = ftell(fptr);
        int move = sizef - sizel;
        fseek(fptr,move,SEEK_CUR);       
    }
    if(ch == '['){
        int sizef = ftell(fptr);
        int flag = 0;
        char chtemp = fgetc(fptr);
        fseek(fptr,-1,SEEK_CUR);
        if(chtemp == '\''){
            flag = 1;
        }
        printf("%c\n",chtemp);
        if(flag==0){
            if(checkendsquarebracket(fptr,size)==1)
                fseek(fptr,-1,SEEK_CUR);
            else{
                printf("ERROR : SQUARE BRACKET is not ended\n");
                exit(1);
            }
        }
        int sizel = ftell(fptr);
        int move = sizef - sizel;
        fseek(fptr,move,SEEK_CUR);
    }
    if(ch == '\''){
        char ch;
        fread(&ch,1,1,fptr);
        int resolved = 0;
        
        /*Simple Valid Cases*/
        if(ch == ' ' || ch == '\'')
            resolved = 1;
        
        /*Escape Sequence*/
        if(ch == '\\'){
            fread(&ch,1,1,fptr);
            if(ch == 'n' || ch == '\n' || ch == ',' || ch == '\r' || ch =='\''){
                resolved = 1;
                fseek(fptr,-2,SEEK_CUR);
            }
        }

        /*Obvious Invalid Case*/
        if(ch == ';' || ch == '\n' || ch == ',' || ch == '\r' || ch == ')'){
            resolved = 1;
        }

        /*Unresolved -> Deeper Validation needed*/
        if(resolved == 0){
            fread(&ch,1,1,fptr);
            int inner_resolved = 0;
            fseek(fptr,-2,SEEK_CUR);
            if(ch == '\'')
                inner_resolved = 1;
            if(inner_resolved == 0){
                int sizef = ftell(fptr);
                int ans = checkendof_singleinvertedcomma(fptr,size);
                int limit = ftell(fptr);
                int count_linenumber = linecount(fptr,limit);
                if(ans == 1){
                    printf("ERROR : Single inverted comma should have single character, multiple character in a single inverted comma is invalid in line %d\n",count_linenumber+1);
                    exit(1);
                }
                else{
                    printf("ERROR : Single inverted comma is not ended in the line,expected single inverted comma is in the line %d\n",count_linenumber + 1);
                    exit(1);
                }
                int sizel = ftell(fptr);
                int move = sizef - sizel;
                fseek(fptr,move,SEEK_CUR);
            }
        }
    }
    if(ch == '{'){
        int sizef = ftell(fptr);
        if(check_end_of_curlybraces(fptr,size)==1){
            fseek(fptr,-1,SEEK_CUR);
        }
        else{
            printf("ERROR : CURLY BRACE is not ended\n");
            exit(1);
        }
        int sizel = ftell(fptr);
        int move = sizef - sizel;
        fseek(fptr,move,SEEK_CUR);
    }
    if(ch == '#'){ //it checks for preprocessor having # as its first character
        int i=0;
        while(1){
            str[i] = ch;
            fread(&ch,1,1,fptr);
            i++;
            if(ch == '\n' || ch == '\r')
                break;
        }
        str[i] = '\0';
        strcpy(tokenvar.lexeme,str);
        tokenvar = group_various_token(tokenvar,fptr,size,prevtok);
        return tokenvar;
    }
    if(ch == '/'){  //it checks for single line comment having // or /* as a mark
        int i=1;
        str[0] = ch;
        fread(&ch,1,1,fptr);
        if(ch == '/'){
            while(1){
                str[i] = ch;
                fread(&ch,1,1,fptr);
                i++;
                if(ch == '\n' || ch == '\r')
                    break;
            }
            str[i] = '\0';
            strcpy(tokenvar.lexeme,str);
            tokenvar = group_various_token(tokenvar,fptr,size,prevtok);
            return tokenvar;
        }
        if(ch == '*'){
            while(1){
                str[i] = ch;
                fread(&ch,1,1,fptr);
                i++;
                if(ch == '*')
                    break;
            }
            fseek(fptr,1,SEEK_CUR);
            str[i] = '\0';
            strcpy(tokenvar.lexeme,str);
            tokenvar = group_various_token(tokenvar,fptr,size,prevtok);
            return tokenvar;
        }
        else{
            fseek(fptr,-2,SEEK_CUR);
        }
    }
    if(ch == '"') //here " is checked for the string literal as its first char
    {
        int i = 0;
        while(1){
            str[i] = ch;
            fread(&ch,1,1,fptr);
            i++;
            if(ch == '"') //loop breaks when there is second " for a string literal
                break;
        }
        str[i] = '"';
        str[i+1] = '\0';
        strcpy(tokenvar.lexeme,str);
        tokenvar = group_various_token(tokenvar,fptr,size,prevtok);
        return tokenvar;
    }
    if(ch >='0' && ch <= '9'){
        int i = 0;
        while(ch >= '0' && ch <= '9'){
            str[i] = ch;
            ch = fgetc(fptr);
            i++;
            if((ch >= '0' && ch <= '9') == 0)
                break;
        }
        str[i]='\0';
        fseek(fptr,-1,SEEK_CUR);
        strcpy(tokenvar.lexeme,str);
        tokenvar = group_various_token(tokenvar,fptr,size,prevtok);
        return tokenvar;
    }
    if((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')){   //check for the char is alphabetic or not
        char ch2;
        str[0] = ch;
        int i=1, identifier_end = 0;
        while(1){
            fread(&ch2,1,1,fptr);
            for(int j=0; j<11; j++){
                if(ch2 == operators[j] || ch2 == ' ' || ch2 == '\''){
                    identifier_end = 1;
                    str[i] = '\0';
                }
            }
            for(int j=0;j<8;j++){
                if(ch2 == specialcharacters[j]){
                    identifier_end = 1;
                    str[i] = '\0';
                }
            }
            if(identifier_end == 1)
                break;
            str[i] = ch2;
            i++;
        }
        str[i]='\0';
        fseek(fptr,-1,SEEK_CUR);
        strcpy(tokenvar.lexeme,str);
    }
    else{
        str[0] = ch;
        str[1] = '\0';
        strcpy(tokenvar.lexeme,str);
    }
    tokenvar = group_various_token(tokenvar,fptr,size,prevtok);
    return tokenvar;
}

int isKeyword(const char*str) //checks whether the passing string is keyword or not
{
    for(int i=0;i<20;i++){
        if(strcmp(str, keywords[i])==0)
            return 1;
    }
    return 0;
}

int isOperator(const char*str) //checks whether the passing string is operator
{
    char ch = str[0];
    for(int i=0;i<9;i++){
        if(ch == operators[i])
            return 1;
    }
    return 0;
}

int isSpecialCharacter(const char*str) //checks whether the passing string is special character or not 
{
    char ch = str[0];
    for(int i=0;i<10;i++){
        if(ch == specialcharacters[i])
            return 1;
    }
    return 0;
}

int isConstant(const char*str){ //checks whether the passing string is constant or not
    char ch = str[0];
    if(ch >= '0' && ch <= '9'){
        return 1;
    }
    return 0;
}

int isliteral(const char *str){ //checks whether the passing string is string literal or not
    char ch = str[0];
    if(ch == '"')
        return 1;
    else
        return 0;
}

int ispreprocessor(const char*str){   //checks whether the string is preprocessor or not
    char ch = str[0];
    if(ch == '#')
        return 1;
    else
        return 0;
}

int iscomment(const char*str){
    char ch = str[0];
    char ch2 = str[1];
    if((ch == '/' && ch2 == '*') || (ch == '/' && ch2 == '/'))
        return 1;
    else
        return 0;
}

int checkendparenthesis(FILE*fptr, int size){
    char c;
    while(1){
        fread(&c,1,1,fptr);
        if(c == ')'){
            return 1;
        }
        if(c == '\n' || c == '\r'){
            int limit = ftell(fptr);
            int linecont = linecount(fptr,limit);
            printf("Expected parenthesis end ) at line %d\n",linecont);
            return 0;
        }
    }
}

int checkendof_singleinvertedcomma(FILE*fptr,int size){
    char ch;
    int start_pos = ftell(fptr);
    while(1){
        fread(&ch,1,1,fptr);
        if(ch == '\'' || ch == '"'){
            return 1;
        }
        if(ch == '\n' || ch == '\r'){
            int limit = ftell(fptr);
            int linecont = linecount(fptr,limit);
            return 0;
        }
    }
}

int checkendof_invertedcomma(FILE*fptr, int size){
    char ch;
    while(1){
        fread(&ch,1,1,fptr);
        if(ch == '"'){
            return 1;
        }
        if(ch == '\n' || ch == '\r'){
            int limit = ftell(fptr);
            int linecont = linecount(fptr,limit);
            printf("Double inverted comma is not found, expected double inverted comma in line %d\n",linecont);
            return 0;
        }
    }
}

int checkendsquarebracket(FILE*fptr,int size){
    char ch;
    while(1){
        fread(&ch,1,1,fptr);
        if(ch == ']'){
            return 1;
        }
        if(ch == '\n' || ch == '\r'){
            int limit = ftell(fptr);
            int linecont = linecount(fptr,limit);
            printf("ERROR: SQUARE BRACKET is not found, expected SQUARE BRACKET in line %d\n",linecont);
            return 0;
        }
    }
}

int check_end_of_curlybraces(FILE*fptr,int size){
    char ch;
    int posstart = ftell(fptr);
    while(1){
        fread(&ch,1,1,fptr);
        if(ch == '}'){
            return 1;
        }
        if(ftell(fptr) >= size){
            rewind(fptr);
            fseek(fptr,posstart,SEEK_CUR);
            int limit = ftell(fptr);
            int linecont = linecount(fptr,limit);
            printf("} is not found, expected to close the curly brace of line %d\n",linecont + 1);
            return 0;
        }
    }
}

int checksemicolon(const char*str, FILE*fptr, int size, Token prevtok)
{
    int posstart = ftell(fptr),semicoloncount = 0,flag = 0,mainflag = 0;
    char ch;
    if(strcmp(str,"int")==0){
        Token sample = checknextToken(fptr,size,prevtok);
        char * strdump = sample.lexeme;
        if(strcmp(strdump,"main")==0){
            mainflag = 1;
        }
    }
    if(mainflag == 0){
        for(int i = 0;i < MAX_STANDARD_WORDS; i++){
            if(strcmp(str,standard_keywords[i])==0)
            {
                flag = 1;
                ch = fgetc(fptr);
                int size = strlen(str);
                fseek(fptr,-(size + 2),SEEK_CUR);
                char cb = fgetc(fptr);
                if(cb == '(' || (strcmp(prevtok.lexeme,"const")==0) || cb == ','){
                    semicoloncount++;
                    break;
                }
                while(ch!='\n'){
                    if(ch == ';')
                        semicoloncount++;
                    if(ch == '{')
                    {
                        semicoloncount++;
                    }
                    ch = fgetc(fptr);
                    if(ch == '\r')
                        continue;
                }
            }
        }
        if(flag == 0){
            return 1;
        }
        if(semicoloncount == 0){
            int limit = ftell(fptr);
            int linecont = linecount(fptr,limit);
            printf("ERROR: Expected ; at the end of the line with token %s at line %d\n",str,linecont);
            exit(1);
        }
    }
    int possend = ftell(fptr);
    int net = possend - posstart;
    fseek(fptr,-net,SEEK_CUR);
}

int linecount(FILE*fptr, int limit)
{
    rewind(fptr);
    int linecount = 0;
    char ch;
    while(ftell(fptr) <= limit){
        ch = fgetc(fptr);
        if(ch == '\n')
            linecount++;
    }
    return linecount;
}

Token group_various_token(Token tokenvar,FILE * fptr,int size,Token prevtok){
    int num = checksemicolon(tokenvar.lexeme,fptr,size,prevtok);
    if(isKeyword(tokenvar.lexeme)==1)
        tokenvar.type = KEYWORD;
    else if(iscomment(tokenvar.lexeme)==1)
        tokenvar.type = COMMENTED_LINE;
    else if(isOperator(tokenvar.lexeme)==1)
        tokenvar.type = OPERATOR;
    else if(isSpecialCharacter(tokenvar.lexeme)==1)
        tokenvar.type = SPECIAL_CHARACTER;
    else if(isConstant(tokenvar.lexeme)==1)
        tokenvar.type = CONSTANT;
    else if(isliteral(tokenvar.lexeme)==1)
        tokenvar.type = LITERAL;
    else if(ispreprocessor(tokenvar.lexeme)==1)
        tokenvar.type = PREPROCESSOR;
    else
        tokenvar.type = IDENTIFIER;
    
    return tokenvar;
}