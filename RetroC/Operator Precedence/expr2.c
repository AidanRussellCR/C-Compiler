#include "defs.h"
#include "data.h"
#include "decl.h"

//Parsing of expressions with full recursive descent

//Parse a primary factor and return an AST node representing it
static struct ASTnode *primary(void){
    struct ASTnode *n;

    //For an INTLIT token, make a leaf AST node and scan in the next token; otherwise, a syntax error for other token types
    switch(Token.token){
        case T_INTLIT:
            n = mkastleaf(A_INTLIT, Token.intvalue);
            scan(&Token);
            return(n);
        default:
            fprintf(stderr, "syntax error on line %d, token %d\n", Line, Token.token);
            exit(1);
    }
}

//Convert a binary operator token into an AST operation
int arithop(int tok){
    switch (tok){
        case T_PLUS:
            return(A_ADD);
        case T_MINUS:
            return(A_SUBTRACT);
        case T_STAR:
            return(A_MULTIPLY);
        case T_SLASH:
            return(A_DIVIDE);
        default:
            fprintf(stderr, "syntax error on line %d, token %d\n", Line, tok);
            exit(1);
    }
}

struct ASTnode *additive_expr(void);

//Return an AST tree whose root is a '*' or '/' binary operator
struct ASTnode *multiplicative_expr(void){
    struct ASTnode *left, *right;
    int tokentype;

    //Get the integer literal on the left; fetch the next token
    left = primary();

    //If no tokens left, return just the left node
    tokentype = Token.token;
    if(Token.token == T_EOF)
        return(left);

    //While the token is a '*' or '/'
    while((tokentype == T_STAR) || (tokentype == T_SLASH)){
        //Fetch the next integer literal
        scan(&Token);
        right = primary();

        //Join that with the left integer literal
        left = mkastnode(arithop(tokentype), left, right, 0);

        //Update the details of the current token; if none left, return the left node
        tokentype = Token.token;
        if(tokentype == T_EOF)
            break;
    }

    //Return whatever tree we have created
    return(left);
}

//Return an AST tree whose root is a '+' or '-' binary operator
struct ASTnode *additive_expr(void){
    struct ASTnode *left, *right;
    int tokentype;

    //Get the left sub-tree at a higher precedence
    left = multiplicative_expr();

    //If no tokens left, return the left node
    tokentype = Token.token;
    if(tokentype == T_EOF)
        return(left);

    //Cache the '+' or '-' token type

    //Loop working on token at our level of precedence
    while(1){
        //Fetch in the next integer literal
        scan(&Token);

        //Get the right sub-tree at a higher precedence
        right = multiplicative_expr();

        //Join the two sub-trees with the low-precedence operator
        left = mkastnode(arithop(tokentype), left, right, 0);

        //Get the next token at our precedence
        tokentype = Token.token;
        if(tokentype == T_EOF)
            break;
    }

    //Return whatever tree that was created
    return(left);
}

struct ASTnode *binexpr(int n){
    return(additive_expr());
}