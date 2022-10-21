#include "defs.h"
#include "data.h"
#include "decl.h"

//Parsing of declarations

//Parse the declaration of a variable
void var_declaration(void){
    //Ensure we have an 'int' token followed by an identifier and a semicolon; text now has the identifiers name and a known identifier
    match(T_INT, "int");
    ident();
    addglob(Text);
    genglobsym(Text);
    semi();
}