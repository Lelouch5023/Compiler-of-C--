%{
	#include "head.h"
	#include "qq.tab.h"	
	#include <stdio.h>
	#include <stdarg.h>	
	
	extern int yylineno;
	
	/*	Global Variable: Symbotable & ErrorStack	*/
	pht SymbolTable;
	pError ErrorStack;
	Code IntermediateCode = NULL;
	int seed_label = 0;
	int seed_v = 0;
	int seed_t = 0;
%}




%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT LP RP LB RB
%nonassoc ELSE

%token INT 
%token FLOAT
%token ID
%token SEMI
%token COMMA
%token ASSIGNOP
%token RELOP
%token PLUS
%token MINUS
%token STAR
%token DIV
%token AND
%token OR
%token DOT
%token NOT
%token TYPE
%token LP
%token RP
%token LB
%token RB
%token LC
%token RC
%token STRUCT
%token RETURN 
%token IF
%token ELSE 
%token WHILE




%token EOP
%%


/* grammar code here */
/********************	High-level Definitions *****************/
Program: ExtDefList {$$ = makenode("Program", 1, $1); 
					treeTrav($$); syntaxAnalysis($$); 
					setFunctionVarNo(); 
					IntermediateCode = translate($$); 
					getMipsCode(IntermediateCode);						
					} 
;
ExtDefList: {/* empty */$$ = NULL;}
		  | ExtDef ExtDefList {$$ = makenode("ExtDefList", 2, $1, $2);}
;
ExtDef: Specifier ExtDecList SEMI {$$ = makenode("ExtDef", 3, $1, $2, $3);}
		| Specifier SEMI {$$ = makenode("ExtDef", 2, $1, $2);}
		| Specifier FunDec CompSt {$$ = makenode("ExtDef", 3, $1, $2, $3);}
;
ExtDecList: VarDec  {$$ = makenode("ExtDecList", 1, $1);}
		  | VarDec COMMA ExtDecList {$$ = makenode("ExtDecList", 3, $1, $2, $3);}
;
/********************   Specifier    **********************/
Specifier: TYPE {$$ = makenode("Specifier", 1, $1);}
		 | StructSpecifier {$$ = makenode("Specifier", 1, $1);}
;
StructSpecifier: STRUCT OptTag LC DefList RC {$$ = makenode("StructSpecifier", 5, $1, $2, $3, $4, $5);}
			   | STRUCT Tag {$$ = makenode("StructSpecifier", 2, $1, $2);}
;
OptTag: {/* empty */$$ = NULL;}
	  | ID {$$ = makenode("OptTag", 1, $1);}
;
Tag: ID  {$$ = makenode("Tag", 1, $1);}
;
/*******************    Declarators   *********************/
VarDec: ID {$$ = makenode("VarDec", 1, $1);}
	  | VarDec LB INT RB {$$ = makenode("VarDec", 4, $1, $2, $3, $4);}
;

FunDec: ID LP VarList RP  {$$ = makenode("FunDec", 4, $1, $2, $3, $4);}
	  | ID LP RP {$$ = makenode("FunDec", 3, $1, $2, $3);}
;
VarList: ParamDec COMMA VarList {$$ = makenode("VarList", 3, $1, $2, $3);}
	   | ParamDec {$$ = makenode("VarList", 1, $1);}
;
ParamDec: Specifier VarDec  {$$ = makenode("ParamDec", 2, $1, $2);}
;
/*******************   Statements   ************************/
CompSt: LC DefList StmtList RC {$$ = makenode("CompSt", 4, $1, $2, $3, $4);}
;
StmtList: {/* empty */$$ = NULL;}
		| Stmt StmtList {$$ = makenode("StmtList", 2, $1, $2);}
;
Stmt: Exp SEMI  {$$ = makenode("Stmt", 2, $1, $2);}
	| CompSt {$$ = makenode("Stmt", 1, $1);}
	| RETURN Exp SEMI {$$ = makenode("Stmt", 3, $1, $2, $3);}
	| IF LP Exp RP Stmt {$$ = makenode("Stmt", 5, $1, $2, $3, $4, $5);}
	| IF LP Exp RP Stmt ELSE Stmt {$$ = makenode("Stmt", 7, $1, $2, $3, $4, $5, $6, $7);}
	| WHILE LP Exp RP Stmt {$$ = makenode("Stmt", 5, $1, $2, $3, $4, $5);}
;
/******************	  Local Definitions  *******************/
DefList: {/* empty */$$ = NULL;}
		| Def DefList {$$ = makenode("DefList", 2, $1, $2);}
;
Def: Specifier DecList SEMI {$$ = makenode("Def", 3, $1, $2, $3);}
;
DecList: Dec {$$ = makenode("DecList", 1, $1);}
	   | Dec COMMA DecList {$$ = makenode("DecList", 3, $1, $2, $3);}
;

Dec: VarDec {$$ = makenode("Dec", 1, $1);}
   | VarDec ASSIGNOP Exp {$$ = makenode("Dec", 3, $1, $2, $3);}
;
/************************  Exp  *****************************/
Exp: Exp ASSIGNOP Exp {$$ = makenode("Exp", 3, $1, $2, $3); }
   | Exp AND Exp {$$ = makenode("Exp", 3, $1, $2, $3); }
   | Exp OR Exp {$$ = makenode("Exp", 3, $1, $2, $3); }
   | Exp RELOP Exp {$$ = makenode("Exp", 3, $1, $2, $3); }
   | Exp PLUS Exp {$$ = makenode("Exp", 3, $1, $2, $3); }
   | Exp MINUS Exp {$$ = makenode("Exp", 3, $1, $2, $3); }
   | Exp STAR Exp {$$ = makenode("Exp", 3, $1, $2, $3); }
   | Exp DIV Exp {$$ = makenode("Exp", 3, $1, $2, $3); }
   | LP Exp RP {$$ = makenode("Exp", 3, $1, $2, $3); }
   | MINUS Exp {$$ = makenode("Exp", 2, $1, $2); }
   | NOT Exp {$$ = makenode("Exp", 2, $1, $2); }
   | ID LP Args RP {$$ = makenode("Exp", 4, $1, $2, $3, $4); }
   | ID LP RP {$$ = makenode("Exp", 3, $1, $2, $3);	}		
   | Exp LB Exp RB {$$ = makenode("Exp", 4, $1, $2, $3, $4); }
   | Exp DOT ID {$$ = makenode("Exp", 3, $1, $2, $3); }			
   | ID {$$ = makenode("Exp", 1, $1); }
   | INT {$$ = makenode("Exp", 1, $1); }
   | FLOAT {$$ = makenode("Exp", 1, $1); }
;
Args: Exp COMMA Args {$$ = makenode("Args", 3, $1, $2, $3); }
	| Exp {$$ = makenode("Args", 1, $1); } 
;







%%
extern FILE* yyin;
int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		if (!(yyin = fopen(argv[1], "r")))
		{
			perror(argv[1]);
			return 1;
		}
	}
	else 
	{
		pnode ast = makenode("Here begins your code:\n", 0);
		printf("name:%s\n", ast->name);
	}
	int tblsize = 100;		/* Size of Symbol Table */
	SymbolTable = newHT(tblsize);
	
	ErrorStack = newErrStack();
	yyparse();
	//pop(ErrorStack);
	printIC(IntermediateCode);
	//optimizeIC(IntermediateCode);
	
	return 0;
}

int yyerror(char* s)
{
	fprintf(stderr, "error at line %d %s\n", yylineno, s);
	return 0;
}





