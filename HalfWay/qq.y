%{
	#include "head.h"
	#include "qq.tab.h"	
	#include <stdio.h>
	#include <stdarg.h>	
	
	extern int yylineno;
	extern pht SymbolTable;
	extern pError ErrorStack;
%}



%nonassoc ELSE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT

%left DOT LP RP LB RB


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
Program: ExtDefList {$$ = makenode("Program", 1, $1); treeTrav($$); syntaxAnalysis($$);}
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
Exp: Exp ASSIGNOP Exp {$$ = makenode("Exp", 3, $1, $2, $3); 
   if (!($1->typesys->kind==BASIC && typeCMP($1->typesys, $3->typesys)==true))
	{
		char buf[128];
		sprintf(buf, "Conflictin types of %s and %s", $1->data, $3->data);
		push(&ErrorStack, buf, $1->line, 7);
	}
	else
	{
		/* type */
		TypeAssign(& $1->typesys, $3->typesys);
		TypeAssign(& $$->typesys, $3->typesys);	// NOT SURE
		/* value */
		strcpy($1->data, $3->data);
		strcpy($$->data, $3->data);
	}
   }
   | Exp AND Exp {$$ = makenode("Exp", 3, $1, $2, $3);
   if (!($1->typesys->kind==BASIC && $1->typesys->u.basic==INT_TYPE && typeCMP($1->typesys, $3->typesys)==true))
	{
		char buf[128];
		sprintf(buf, "Conflictin types of %s and %s", $1->data, $3->data);
		push(&ErrorStack, buf, $1->line, 7);
	}
	else
	{
		/* type */
		TypeAssign(& $$->typesys, $1->typesys);
		/* value */
		int val1 = atoi($1->data);
		int val2 = atoi($2->data);
		itoa(val1 && val2, $$->data, 10);
	} 
   }
   | Exp OR Exp {$$ = makenode("Exp", 3, $1, $2, $3);
   if (!($1->typesys->kind==BASIC && $1->typesys->u.basic==INT_TYPE && typeCMP($1->typesys, $3->typesys)==true))
	{
		char buf[128];
		sprintf(buf, "Conflictin types of %s and %s", $1->data, $3->data);
		push(&ErrorStack, buf, $1->line, 7);
	}
	else
	{
		/* type */
		TypeAssign(& $$->typesys, $1->typesys);
		/* value */
		int val1 = atoi($1->data);
		int val2 = atoi($2->data);
		itoa(val1 || val2, $$->data, 10);
	}
   }
   | Exp RELOP Exp {$$ = makenode("Exp", 3, $1, $2, $3);
   if (!($1->typesys->kind==BASIC && typeCMP($1->typesys, $3->typesys)==true))
	{
		char buf[128];
		sprintf(buf, "Conflictin types of %s and %s", $1->data, $3->data);
		push(&ErrorStack, buf, $1->line, 7);
	}
	else
	{
		/* type */
		$$->typesys->kind = BASIC;
		$$->typesys->u.basic = INT_TYPE;
		/* value */
		int ret = 0;
		if ($1->typesys->u.basic == INT_TYPE)	// INT_TYPE RELOP
		{
			int val1 = atoi($1->data);
			int val2 = atoi($3->data);
			if (strcmp($2->data, ">")==0)
				ret = (val1 > val2);
			if (strcmp($2->data, "<")==0)
				ret = (val1 < val2);
			if (strcmp($2->data, "==")==0)
				ret = (val1 == val2);
			if (strcmp($2->data, ">=")==0)
				ret = (val1 >= val2);
			if (strcmp($2->data, "<=")==0)
				ret = (val1 <= val2);
			if (strcmp($2->data, "!=")==0)
				ret = (val1 != val2);

		}
		else	// FLOAT_TYPE RELOP
		{
			double val1 = atof($1->data);
			double val2 = atof($3->data);
			if (strcmp($2->data, ">")==0)
				ret = (val1 > val2);
			if (strcmp($2->data, "<")==0)
				ret = (val1 < val2);
			if (strcmp($2->data, "==")==0)
				ret = (val1 == val2);
			if (strcmp($2->data, ">=")==0)
				ret = (val1 >= val2);
			if (strcmp($2->data, "<=")==0)
				ret = (val1 <= val2);
			if (strcmp($2->data, "!=")==0)
				ret = (val1 != val2);

		}
		itoa(ret, $$->data, 10);
	}	
   }
   | Exp PLUS Exp {$$ = makenode("Exp", 3, $1, $2, $3);
   if (!($1->typesys->kind==BASIC && typeCMP($1->typesys, $3->typesys)==true))
	{
		char buf[128];
		sprintf(buf, "Conflictin types of %s and %s", $1->data, $3->data);
		push(&ErrorStack, buf, $1->line, 7);
	}
	else
	{
		/* type */
		TypeAssign(& $$->typesys, $1->typesys);
		/* value */
		if ($1->typesys->u.basic == INT_TYPE)	// INT_TYPE
		{
			int ret = 0;
			int val1 = atoi($1->data);
			int val2 = atoi($3->data);
			ret = val1 + val2;
			itoa(ret, $$->data, 10);
		}
		else	// FLOAT_TYPE
		{
			double ret = 0.0;
			double val1 = atof($1->data);
			double val2 = atof($3->data);
			ret = val1 + val2;
			sprintf($$->data, "%*lf", DATALEN-1, ret);
		}
	}	
	}
   }
   | Exp MINUS Exp {$$ = makenode("Exp", 3, $1, $2, $3);
    if (!($1->typesys->kind==BASIC && typeCMP($1->typesys, $3->typesys)==true))
	{
		char buf[128];
		sprintf(buf, "Conflictin types of %s and %s", $1->data, $3->data);
		push(&ErrorStack, buf, $1->line, 7);
	}
	else
	{
		/* type */
		TypeAssign(& $$->typesys, $1->typesys);
		/* value */
		if ($1->typesys->u.basic == INT_TYPE)	// INT_TYPE 
		{
			int ret = 0;
			int val1 = atoi($1->data);
			int val2 = atoi($3->data);
			ret = val1 - val2;
			itoa(ret, $$->data, 10);
		}
		else	// FLOAT_TYPE
		{
			double ret = 0.0;
			double val1 = atof($1->data);
			double val2 = atof($3->data);
			ret = val1 - val2;
			sprintf($$->data, "%*lf", DATALEN-1, ret);
		}
	}
   }
   | Exp STAR Exp {$$ = makenode("Exp", 3, $1, $2, $3);
   if (!($1->typesys->kind==BASIC && typeCMP($1->typesys, $3->typesys)==true))
	{
		char buf[128];
		sprintf(buf, "Conflictin types of %s and %s", $1->data, $3->data);
		push(&ErrorStack, buf, $1->line, 7);
	}
	else
	{
		/* type */
		TypeAssign(& $$->typesys, $1->typesys);
		/* value */
		if ($1->typesys->u.basic == INT_TYPE)	// INT_TYPE
		{
			int ret = 0;
			int val1 = atoi($1->data);
			int val2 = atoi($3->data);
			ret = val1 * val2;
			itoa(ret, $$->data, 10);
		}
		else	// FLOAT_TYPE 
		{
			double ret = 0.0;
			double val1 = atof($1->data);
			double val2 = atof($3->data);
			ret = val1 * val2;
			sprintf($$->data, "%*lf", DATALEN-1, ret);
		}
	}	
   }
   | Exp DIV Exp {$$ = makenode("Exp", 3, $1, $2, $3);
    if (!($1->typesys->kind==BASIC && typeCMP($1->typesys, $3->typesys)==true))
	{
		char buf[128];
		sprintf(buf, "Conflictin types of %s and %s", $1->data, $3->data);
		push(&ErrorStack, buf, $1->line, 7);
	}
	else
	{
		/* type */
		TypeAssign(& $$->typesys, $1->typesys);
		/* value */
		if ($1->typesys->u.basic == INT_TYPE)	// INT_TYPE
		{
			int ret = 0;
			int val1 = atoi($1->data);
			int val2 = atoi($3->data);
			if (val2==0)
			{
				char buf[128];
				sprintf(buf, "Zero Division Exception");
				push(&ErrorStack, buf, $1->line, 7);	
				strcpy($$->data, "0");
			}
			ret = val1 / val2;
			itoa(ret, $$->data, 10);
		}
		else	// FLOAT_TYPE 
		{
			double ret = 0.0;
			double val1 = atof($1->data);
			double val2 = atof($3->data);
			if (val2==0.0)
			{
				char buf[128];
				sprintf(buf, "Zero Division Exception");
				push(&ErrorStack, buf, $1->line, 7);	
				strcpy($$->data, "0");
			}
			ret = val1 / val2;
			sprintf($$->data, "%*lf", DATALEN-1, ret);
		}
	}
   }
   | LP Exp RP {$$ = makenode("Exp", 3, $1, $2, $3);
	
   }
   | MINUS Exp {$$ = makenode("Exp", 2, $1, $2);
   if (!($2->typesys->kind==BASIC))
	{
		char buf[128];
		sprintf(buf, "Conflictin types of %s with MINUS", $2->data);
		push(&ErrorStack, buf, $2->line, 7);
	}
	else
	{
		/* type */
		TypeAssign(& $$->typesys, $2->typesys);
		/* value */
		if ($2->typesys->kind == INT_TYPE)	//INT_TYPE
		{
			int ret = 0;
			int val = atoi($2->data);
			ret = - val;
			sprintf($$->data, "%d", ret);
		}
		else				//FLOAT_TYPE
		{
			double ret = 0.0;
			double val = atof($2->data);
			ret = - val;
			sprintf($$->data, "%*lf", DATALEN-1, ret);
		}
	}

   }
   | NOT Exp {$$ = makenode("Exp", 2, $1, $2);
   if (!($2->typesys->kind==BASIC && $2->typesys->u.basic==INT_TYPE ))
	{
		char buf[128];
		sprintf(buf, "Conflictin types of %s with NOT", $2->data);
		push(&ErrorStack, buf, $1->line, 7);
	}
	else
	{
		/* type */
		TypeAssign(& $$->typesys, $2->typesys);
		/* value */
		int ret = 0;
		int val = atoi($2->data);
		ret = ! val;
		sprintf($$->data, "%d", ret);
	} 
   }
   | ID LP Args RP {$$ = makenode("Exp", 4, $1, $2, $3, $4);	/* Function Call! */
   
   }
   | ID LP RP {$$ = makenode("Exp", 3, $1, $2, $3);				/* Function Call! */
   
   
   }		
   | Exp LB Exp RB {$$ = makenode("Exp", 4, $1, $2, $3, $4);	/* Array Access! */
   
   }
   | Exp DOT ID {$$ = makenode("Exp", 3, $1, $2, $3);			/* Struct Access! */	//TODO: OOP
   
   }			
   | ID {$$ = makenode("Exp", 1, $1); 
		/* look for it from SymbolTable */		
		if ()	//NOT FOUND FROM HASHTABLE
		{

		}
		else
		{
			/* type */
			TypeAssign($$->typesys, $1->typesys);
			/* value */
			//strcpy($$->data, $1->data);		//TODO: PUZZLE, look up table or directly assign?

		}
   }
   | INT {$$ = makenode("Exp", 1, $1);
	/* type */
	TypeAssign($$->typesys, $1->typesys);
	/* value */
	strcpy($$->data, $1->data);

   }
   | FLOAT {$$ = makenode("Exp", 1, $1);
   	/* type */
	TypeAssign($$->typesys, $1->typesys);
	/* value */
	strcpy($$->data, $1->data);

   }
;
Args: Exp COMMA Args {$$ = makenode("Args", 3, $1, $2, $3);}
	| Exp {$$ = makenode("Args", 1, $1);}
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
	pht SymbolTable = newHT(tblsize);
	pError ErrorStack = newErrStack();
	yyparse();
	
	return 0;
}

int yyerror(char* s)
{
	fprintf(stderr, "error at line %d %s\n", yylineno, s);
	return 0;
}





