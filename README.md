# R-language-compiler
R is a language and environment for statistical computing and graphics. This is a compiler for it built in C.
All font end stages included :
 - Grammar ( LL(1) )
 - Lexical analysis (Scanner)
 - Syntactic analysis (Parser)
 - Semantic analysis
 
 
 
 
Used Grammar :

Prog :      	[expr | Loop | Function | Decision]*
expr:     	 id <- ( id | CallFunction | data )  [ operator (id | CallFunction | data) ]*
            		| id <- function
            		| ( id | CallFunction | data )  [ operator (id | CallFunction | data) ]* -> id
operation:  	 ( id | CallFunction | data ) operator (id | CallFunction | data )
Operator :  	== | < | > | + | - | * | /
AssignOp :  	<- | = | ->
data     :  	INT | FLOAT | STRING | VECTOR
Decision:   	if ( condition ) { expr }
           	     	| if ( condition ) { expr } else { expr }
Loop:       	while ( condition ) { expr }
            |   repeat { expr }
            |   for ( ID in vector ) { expr }
vector :    	c( id [, id]* )
Function:    	function ( None | id [, id ]* ) { expr | return }
CallFunc   	id ( None | data [,data]* )
