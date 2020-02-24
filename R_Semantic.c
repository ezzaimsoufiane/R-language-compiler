#ifndef Already
    #include "globals.h"
#endif

#define UNDECLARED  100
/* Errors of Semantic

    Type mismatch
    Undeclared variable
    Number of arguments in function

*/

/// ERRORSemantic
void ERRORSemantic(char* ErrorText){
    printf("\n\n %s \t", ErrorText);
    printf("Semantic Error ");
    printf("\t in line : %d \n\n" , line);
    exit(1);
}
// Check if the variable to assign is declared
void SemanticID(Symbol Input, Symbol Variable){
    int a=0,b=0;
    // If the input variable is not declared, it's an error
    if( (a = isIDDeclared(Input)) == UNDECLARED )
        ERRORSemantic("Undeclared Variable");
    //
    if( (b = isIDDeclared(Variable)) == UNDECLARED){
        // Add this Symbol to the table
        strcpy (SymbTable[nSymbol].Name , Variable.Name);
        strcpy (SymbTable[nSymbol].Value , SymbTable[a].Value);
        SymbTable[nSymbol].Type = SymbTable[a].Type;
        nSymbol++;
    // If the variable is already declared, modify its information
    }else{
        strcpy (SymbTable[b].Value , SymbTable[a].Value);
        SymbTable[b].Type = SymbTable[a].Type;
    }

}
// Add variable value to semantic table, or change its information if already declared
void SemanticData(Symbol Input, Symbol Variable){
    int b=0;
    if( (b = isIDDeclared(Variable)) == UNDECLARED){
        // Add this Symbol to the table
        strcpy(SymbTable[nSymbol].Name , Variable.Name);
        strcpy (SymbTable[nSymbol].Value , Input.Value);
        SymbTable[nSymbol].Type = Input.Type;
        nSymbol++;
    // If the variable is already declared, modify its information
    }else{
        strcpy (SymbTable[b].Value , Input.Value);
        SymbTable[b].Type = Input.Type;
    }
}

//
int isIDDeclared(Symbol token){
    int n = 0;
    while( strcmp( SymbTable[n].Name, token.Name ) && n < nSymbol )
        n++;

    // If Not declared
    if(n == nSymbol) return UNDECLARED;
    // If already declared
    return n;
}

void printSymbolTable(){
    int n = 0;
    while( n < nSymbol){
        printf("Name  :%s\tValue  :%s\tType  :%s\n", SymbTable[n].Name , SymbTable[n].Value , Token_String[SymbTable[n].Type]  );
        n++;
    }
}
