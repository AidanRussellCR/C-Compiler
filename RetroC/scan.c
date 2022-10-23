#include "defs.h"
#include "data.h"
#include "decl.h"

//Lexical Scanner

//Return position of character c in string s; return -1 if not found
static int chrpos(char *s, int c){
    char *p;

    p = strchr(s, c);
    return(p ? p - s : -1);
}

//Get next character from input file
static int next(void){
    int c;

    if(Putback){        //Use the character put
        c = Putback;    //Back if there is one
        Putback = 0;
        return(c);
    }

    c = fgetc(Infile);  //Read from input file
    if('\n' == c)
        Line++;         //Increment line count
    return(c);
}

//Put back an unwanted character
static void putback(int c){
    Putback = c;
}

//Skip past input that we don't need to deal with, such as whitespace, newlines; return the first character we need to deal with
static int skip(void){
    int c;

    c = next();
    while(' ' == c || '\t' == c || '\n' == c || '\r' == c || '\f' == c){
        c = next();
    }
    return (c);
}

//Scan and return an integer literal value from the input file; Store the value as a string in text
static int scanint(int c){
    int k, val = 0;

    //Convert each character into an int value
    while((k = chrpos("0123456789", c)) >= 0){
        val = val * 10 + k;
        c = next();
    }

    //If hitting non-integer character, put it back
    putback(c);
    return val;
}

//Scan an identifier from the input file and store it in buf[]; return the identifier's length
static int scanident(int c, char *buf, int lim){
    int i = 0;

    //Allow digits, alpha and underscores
    while(isalpha(c) || isdigit(c) || '_' == c){
        //Error if identifier length limit is hit, else append to buf[] and get next character
        if(lim - 1 == i){
            fatal("identifier too long");
        } else if(i < lim - 1){
            buf[i++] = c;
        }
        c = next();
    }
    //We hit a non-valid character, put it back; NUL-terminate with buf[] and return the length
    putback(c);
    buf[i] = '\0';
    return(i);
}

//Given a word from the input, return the matching keyword token number or 0 if it's not a keyword;
//switch on the first letter so that no time is wasted strcmp()ing against all keywords
static int keyword(char *s){
    switch(*s){
        case 'e':
            if(!strcmp(s, "else"))
                return(T_ELSE);
            break;
        case 'i':
            if(!strcmp(s, "if"))
                return(T_IF);
            if(!strcmp(s, "int"))
                return(T_INT);
            break;
        case 'p':
            if(!strcmp(s, "print"))
                return(T_PRINT);
            break;
        case 'w':
            if(!strcmp(s, "while"))
                return(T_WHILE);
            break;
    }
    return(0);
}

//Scan and return the next token found in the input; Return 1 if token is valid, 0 if no tokens left
int scan(struct token *t){
    int c, tokentype;

    //Skip whitespace
    c = skip();

    //Determine the token based on the input character
    switch(c){
        case EOF:
            t->token = T_EOF;
            return(0);
        case '+':
            t->token = T_PLUS;
            break;
        case '-':
            t->token = T_MINUS;
            break;
        case '*':
            t->token = T_STAR;
            break;
        case '/':
            t->token = T_SLASH;
            break;
        case ';':
            t->token = T_SEMI;
            break;
        case '{':
            t->token = T_LBRACE;
            break;
        case '}':
            t->token = T_RBRACE;
            break;
        case '(':
            t->token = T_LPAREN;
            break;
        case ')':
            t->token = T_RPAREN;
            break;
        case '=':
            if((c = next()) == '='){
                t->token = T_EQ;
            } else {
                putback(c);
                t->token = T_ASSIGN;
            }
            break;
        case '!':
            if((c = next()) == '='){
                t->token = T_NE;
            } else {
                fatalc("Unrecognized character", c);
            }
            break;
        case '<':
            if((c = next()) == '='){
                t->token = T_LE;
            } else {
                putback(c);
                t->token = T_LT;
            }
            break;
        case '>':
            if((c = next()) == '='){
                t->token = T_GE;
            } else {
                putback(c);
                t->token = T_GT;
            }
            break;
        default:
            //If it's a digit, scan the literal integer value
            if(isdigit(c)){
                t->intvalue = scanint(c);
                t->token = T_INTLIT;
                break;
            } else if(isalpha(c) || '_' == c){
                //Read in a keyword or identifier
                scanident(c, Text, TEXTLEN);

                //If it's a recognised keyword, return that token
                if(tokentype = keyword(Text)){
                    t->token = tokentype;
                    break;
                }
                //Not a recognised keyword, so it must be an identifier
                t->token = T_IDENT;
                break;
            }
            //The character isn't a part of any recognised token; error
            fatalc("Unrecognised character", c);
    }

    //We found a token
    return(1);
}