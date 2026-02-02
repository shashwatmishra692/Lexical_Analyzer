#include <stdio.h>
#include "lexer.h"
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char*argv[]){
    if(argc==1 || argc > 2){
        printf("ERROR: Command line arguments need to be passed as ./a.out <filename>\n");
        return 1;
    }
    FILE*fptr = initiatelexer(argv[1]);
    fseek(fptr,0,SEEK_END);
    int size = ftell(fptr);
    printf("passed file size %d\n",size);
    fseek(fptr,0,SEEK_SET);
    Token token,prevtok;
    while((token = checknextToken(fptr,size,prevtok)).type != UNKNOWN){
        prevtok = token;
        int id = token.type;
        switch(id){
            case 0: printf("Type: KEYWORD   "); break;
            case 1: printf("Type: OPERATOR  "); break;
            case 2: printf("Type: SPECIAL CHARACTER "); break;
            case 3: printf("Type: CONSTANT          "); break;
            case 4: printf("Type: IDENTIFIER        "); break;
            case 5: printf("Type: LITERAL           "); break;
            case 6: printf("Type: PREPROCESSOR      "); break;
            case 7: printf("Type: COMMENTED_LINE    "); break;
            default: printf("default\t");
        }
        int limit = ftell(fptr);
        int linecont = linecount(fptr,limit);
        printf("------> Token: %-23s line %d\n",token.lexeme,linecont + 1);
        if(ftell(fptr) >= size)
            break;
    }
    printf("Parsing : %s : Done\n",argv[1]);
    return 0;
}