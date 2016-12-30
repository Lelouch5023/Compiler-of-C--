#include "head.h"
#ifndef HEAD_H
/*
 *  新表，newHT
 *	插入，insertHT
 *	查找，getHT
 *	删除，delHT
 */

int insertHT(pht H, char* name, Type typesys);
int getHT(pht H, char* name, Type typesys, int* index, int* forward);
int delHT(pht H, char* name, Type typesys);

#endif
//#define TEST_DFVISIT		/* Switch for Test */
#define Test

extern pht SymbolTable;
extern pError ErrorStack;

void dfvisit(pnode t)
{
	if (t==NULL || t->type==1)	//For empty nodes or leaf nodes, just return 
		return ;

	int i = 0;
	int N = t->childnum;


/*
 *	Here begins my Inherit Attributes' Calculations
 *  for each child. top-donw method.
 */
	
	
	/********************	High-level Definitions *****************/
	if (strcmp(t->name, "Program")==0)
	{
	//>> Program: ExtDefList
				
	}
	else if (strcmp(t->name, "ExtDefList")==0)
	{
	//>> ExtDefList: ExtDef ExtDefList
		if (N==2)
		{
			
		}
	//>> ExtDefList: eps
		else 
		{

		}
	}
	else if (strcmp(t->name, "ExtDef")==0)
	{																			
		dfvisit(t->child[i++]);	//**// dfvisit(Specifier);
	//>> ExtDef: Specifier ExtDecList SEMI
		if (N==3 && strcmp(t->child[1]->name, "ExtDecList")==0)
		{
			t->child[1]->typesys = t->child[0]->typesys;	//ExtDecList.type := Specifier.type

		}
	//>> ExtDef: Specifier FunDec CompSt
		else if (N==3 && strcmp(t->child[1]->name, "FunDec")==0)
		{
			dfvisit(t->child[i++]);	/* This visits FunDec */
		
			t->typesys->kind = FUNCTION;
			/* function: retrun value, parameters, function name */
			TypeAssign(& t->typesys->u.function.ret, t->child[0]->typesys); 	//t->typesys->u.function.ret := Specifier->typesys
			fieldlistAssign(& t->typesys->u.function.param, t->child[1]->typesys->u.function.param);	//t->typesys->u.function.param := FunDec->typesys->u.funtion.param
			strcpy(t->typesys->u.function.funcname, t->child[1]->typesys->u.function.funcname);				//t->typesys->u.function.funcname := FunDec->data
			/* insert symbol table */
			insertHT(SymbolTable, t->typesys->u.function.funcname, t->typesys);
		}
	//>> ExtDef: Specifier SEMI
		else
		{

		}
	
	}
	else if (strcmp(t->name, "ExtDecList")==0)
	{
	//>> ExtDecList: VarDec
		if (N==1)
		{
			TypeAssign(& t->child[0]->typesys, t->typesys);	//VarDec.type := ExtDecList.type
		}
	//>> ExtDecList 1: VarDec COMMA ExtDecList 2
		else
		{
			TypeAssign(& t->child[0]->typesys, t->typesys);	//VarDec.type := ExtDecList1.type
			TypeAssign(& t->child[2]->typesys, t->typesys);	//ExtDecList2.type := ExtDecList1.type
		}
	}
	/********************   Specifier    **********************/
	else if (strcmp(t->name, "Specifier")==0)
	{
	//>> Specifier: TYPE
		if (N==1 && strcmp(t->child[0]->name, "TYPE")==0)
		{
			t->typesys = t->child[0]->typesys;

		}
	//>> Specifier: StructSpecifier
		else
		{
			dfvisit(t->child[i++]);
			t->typesys = t->child[0]->typesys;
		}
		
	}
	else if (strcmp(t->name, "StructSpecifier")==0)
	{
	//>> StructSpecifier: STRUCT OptTag LC DefList RC
		if (N==5)
		{	
			t->typesys->kind = OBJECT;
			dfvisit(t->child[1]);	/* fixed */
			dfvisit(t->child[3]);	/* fixed */
			strcpy(t->typesys->u.object.structname, t->child[1]->data);
			t->typesys->u.object.structfield = t->child[3]->typesys->u.structure;
			
			Type tp = (Type) calloc(1, sizeof(struct Type_));
			tp->kind = STRUCTURE;
			tp->u.structure = t->child[3]->typesys->u.structure;
			char* name = t->child[1]->data;
			insertHT(SymbolTable, name, tp);

			return ; /* early-stopping */
		}
	//>> StructSpecifier: STRUCT Tag
		else
		{
			dfvisit(t->child[1]);	/* fixed */
			t->typesys->kind = OBJECT;
			strcpy(t->typesys->u.object.structname, t->child[1]->data);
			//t->typesys->u.object.structfield	//TODO

			return ; /* early-stopping */
		}
	}
	else if (strcmp(t->name, "OptTag")==0)
	{
	//>> OptTag: ID
		if (t->child[0]!=NULL)
		{
			strcpy(t->data, t->child[0]->data);
		}
	//>> OptTag: eps
		else
		{
			t->data[0] = 0;	// as a "" string
		}
	}
	else if (strcmp(t->name, "Tag")==0)
	{
	//>> Tag: ID
		strcpy(t->data, t->child[0]->data);
	}
	/*******************    Declarators   *********************/
	else if (strcmp(t->name, "VarDec")==0)
	{
	//>> VarDec: ID
		if (N==1)
		{
			strcpy(t->data, t->child[0]->data);
			TypeAssign(& t->child[0]->typesys, t->typesys); 

			insertHT(SymbolTable, t->child[0]->data, t->child[0]->typesys);
		}
	//>> VarDec1: VarDec2 LB INT RB
		else
		{
			dfvisit(t->child[0]), i++;

			//VarDec2->typesys = toArray(VarDec1->typesys, INT.size);
			t->child[0]->typesys->kind = ARRAY;
			t->child[0]->typesys->u.array.size = atoi(t->child[2]->data);
			t->child[0]->typesys->u.array.elem = t->typesys;

			
			strcpy(t->data, t->child[0]->data);
			//TypeAssign(& t->typesys, t->child[0]->typesys);
			/* Note VarDec1 has a type inherited from Specifier, so we pass this to VarDec2 */
		}
	}
	else if (strcmp(t->name, "FunDec")==0)
	{
	//>> FunDec: ID LP VarList RP
		if (N==4)
		{
			dfvisit(t->child[2]);
			t->typesys->kind = FUNCTION;
			//fieldlistAssign(& t->typesys->u.function.param, t->child[2]->typesys->u.structure);//VarList is a structure!
			t->typesys->u.function.param = t->child[2]->typesys->u.structure;
			strcpy(t->typesys->u.function.funcname, t->child[0]->data);	//ID name

			return ; /* early-stopping */
		}
	//>> FunDec: ID LP RP
		else
		{
			t->typesys->kind = FUNCTION;
			t->typesys->u.function.param = NULL;
			strcpy(t->typesys->u.function.funcname, t->child[0]->data);	//ID name
	
		}
	}
	else if (strcmp(t->name, "VarList")==0)
	{
	//>> VarList: ParamDec COMMA VarList
		if (N==3)
		{
			while(i < N)
			dfvisit(t->child[i++]);

			t->typesys->kind = STRUCTURE;

			FieldList f = (FieldList) calloc(1, sizeof(struct FieldList_));
			f->name = t->child[0]->data; 							//name
			TypeAssign(& f->type, t->child[0]->typesys);   		//type
			f->next = t->child[2]->typesys->u.structure ;		//next

			t->typesys->u.structure = f;

			/* insert symbol table */
			//insertHT(SymbolTable, f->name, f->type);

			if (i >= N)	/* early-stopping */
				return ;	
		}
	//>> VarList: ParamDec
		else
		{
			dfvisit(t->child[i++]);
			t->typesys->kind = STRUCTURE;
			
			FieldList f = (FieldList) calloc(1, sizeof(struct FieldList_));
			f->name = t->child[0]->data;			 						//name
			TypeAssign(& f->type, t->child[0]->typesys); 	//type
			f->next = NULL;									//next

			t->typesys->u.structure = f;

			/* insert symbol table */
			//insertHT(SymbolTable, f->name, f->type);
			if (i>=N)	/* early-stopping */
				return ;
		}
	}
	else if (strcmp(t->name, "ParamDec")==0)
	{
	//>> ParamDec: Specifier VarDec
		dfvisit(t->child[i++]);
		t->child[1]->typesys = t->child[0]->typesys;
		
		dfvisit(t->child[i++]);
		pnode tmp = t->child[1];
		while(tmp!=NULL && strcmp(tmp->child[0]->name, "ID")!=0)
			tmp = tmp->child[0];

		strcpy(t->data, t->child[1]->data);
		TypeAssign(& t->typesys, tmp->typesys);

		

	}	
	
	/*******************   Statements   ************************/
	else if (strcmp(t->name, "CompSt")==0)
	{
	//>> CompSt: LC DefList StmtList RC
	
	}	
	else if (strcmp(t->name, "StmtList")==0)
	{
	//>> StmtList: Stmt StmtList
	//>> StmtList: eps
	}	
	else if (strcmp(t->name, "Stmt")==0)
	{
	//>> Stmt: Exp SEMI
	
	//>> Stmt: CompSt
	
	//>> Stmt: RETURN Exp SEMI
	
	//>> Stmt: IF LP Exp RP Stmt
	
	//>> Stmt: IF LP Exp RP Stmt ELSE Stmt
	
	//>> Stmt: WHILE LP Exp RP Stmt
	
	}
	/******************	  Local Definitions  *******************/
	else if (strcmp(t->name, "DefList")==0)
	{
	//>> DefList: Def DefList
		if (N==2)
		{/* DefList = [Def, Def, Def ...],  every Def is a fieldlist */
			dfvisit(t->child[i++]);
			dfvisit(t->child[i++]);

			//t->child[0]->typesys->kind = STRUCTURE;
			t->typesys->kind = STRUCTURE;
			FieldList f = (FieldList) calloc(1, sizeof(struct FieldList_));
			t->typesys->u.structure = f;
			//t->child[0]->typesys->u.structure = f;
			

			f->name = (char *) calloc(DATALEN, sizeof(char));//, strcpy(f->data, t->child[0]->data);
			f->type = t->child[0]->typesys;
			f->next = (t->child[1]==NULL) ? NULL : t->child[1]->typesys->u.structure;

			if (i>=N)	/* early-stopping */
				return ;
		}
	//>> DefList: eps
		else
		{
			TypeAssign(& t->typesys, NULL);
		}
	}
	else if (strcmp(t->name, "Def")==0)
	{
	//>> Def: Specifier DecList SEMI
		dfvisit(t->child[i++]);
		t->child[1]->typesys = t->child[0]->typesys;
		
		t->typesys = t->child[0]->typesys;	/* This is for structure field list! */
			
	}
	else if (strcmp(t->name, "DecList")==0)
	{
	//>> DecList: Dec
		if (N==1)
		{


			TypeAssign(& t->child[0]->typesys, t->typesys);


		}
	//>> DecList: Dec COMMA DecList
		else
		{



			TypeAssign(& t->child[0]->typesys, t->typesys);
			TypeAssign(& t->child[2]->typesys, t->typesys);



		}
	}
	else if (strcmp(t->name, "Dec")==0)
	{
	//>> Dec: VarDec
		if (N==1)
		{
			strcpy(t->data, t->child[0]->data);
			TypeAssign(& t->child[0]->typesys, t->typesys);
		}
	//>> Dec: VarDec ASSIGNOP Exp
		else
		{

			strcpy(t->data, t->child[0]->data);
			TypeAssign(& t->child[0]->typesys, t->typesys);

			#define COND int
			#ifdef TEST
			#define COND 0

			if (COND)	/* Exp must have a pre-type */
			{
				char buf[128];
				sprintf(buf, "Conflictin types of %s and %s", t->child[0]->data, t->child[2]->data);
				push(&ErrorStack, buf, t->child[0]->line, 7);
			}

			#endif
		}
	}
	/************************  Exp  *****************************/
	else if (strcmp(t->name, "Exp")==0)
	{
	//>> Exp: Exp ASSIGNOP Exp
		if (N==3 && strcmp(t->child[1]->name, "ASSIGNOP")==0)
		{
			if (!(t->child[0]->typesys->kind==BASIC && typeCMP(t->child[0]->typesys, t->child[2]->typesys)==true))
			{
				char buf[128];
				sprintf(buf, "Conflictin types of %s and %s", t->child[0]->data, t->child[2]->data);
				push(&ErrorStack, buf, t->child[0]->line, 7);
			}
			TypeAssign(&(t->typesys), t->child[0]->typesys);
		}
		else
	//>> Exp: Exp AND Exp
		if (N==3 && strcmp(t->child[1]->name, "AND")==0)
		{
			if (!(t->child[0]->typesys->kind==BASIC && t->child[0]->typesys->u.basic==INT_TYPE 
				&& typeCMP(t->child[0]->typesys, t->child[2]->typesys)==true))
			{
				char buf[128];
				sprintf(buf, "Conflictin types of %s and %s", t->child[0]->data, t->child[2]->data);
				push(&ErrorStack, buf, t->child[0]->line, 7);
			}
			TypeAssign(&(t->typesys), t->child[0]->typesys);
		}
		else
	//>> Exp: Exp OR Exp
		if (N==3 && strcmp(t->child[1]->name, "OR")==0)
		{
			if (!(t->child[0]->typesys->kind==BASIC && t->child[0]->typesys->u.basic==INT_TYPE 
				&& typeCMP(t->child[0]->typesys, t->child[2]->typesys)==true))
			{
				char buf[128];
				sprintf(buf, "Conflictin types of %s and %s", t->child[0]->data, t->child[2]->data);
				push(&ErrorStack, buf, t->child[0]->line, 7);
			}
			TypeAssign(&(t->typesys), t->child[0]->typesys);
		}
		else
	//>> Exp: Exp RELOP Exp
		if (N==3 && strcmp(t->child[1]->name, "RELOP")==0)
		{
			if (!(t->child[0]->typesys->kind==BASIC 
				&& typeCMP(t->child[0]->typesys, t->child[2]->typesys)==true))
			{
				char buf[128];
				sprintf(buf, "Conflictin types of %s and %s", t->child[0]->data, t->child[2]->data);
				push(&ErrorStack, buf, t->child[0]->line, 7);
			}

			Type ty = (Type) calloc(1, sizeof(struct Type_));	// should assign INT_TYPE to parent
			ty->kind = BASIC; ty->u.basic = INT_TYPE;
			TypeAssign(&(t->typesys), ty);
			free(ty);
		}
		else
	//>> Exp: Exp PLUS Exp
		if (N==3 && strcmp(t->child[1]->name, "PLUS")==0)
		{
			if (!(t->child[0]->typesys->kind==BASIC 
				&& typeCMP(t->child[0]->typesys, t->child[2]->typesys)==true))
			{
				char buf[128];
				sprintf(buf, "Conflictin types of %s and %s", t->child[0]->data, t->child[2]->data);
				push(&ErrorStack, buf, t->child[0]->line, 7);
			}
			TypeAssign(&(t->typesys), t->child[0]->typesys);     
		}
		else
	//>> Exp: Exp MINUS Exp
		if (N==3 && strcmp(t->child[1]->name, "MINUS")==0)
		{
			if (!(t->child[0]->typesys->kind==BASIC 
				&& typeCMP(t->child[0]->typesys, t->child[2]->typesys)==true))
			{
				char buf[128];
				sprintf(buf, "Conflictin types of %s and %s", t->child[0]->data, t->child[2]->data);
				push(&ErrorStack, buf, t->child[0]->line, 7);
			}
			TypeAssign(&(t->typesys), t->child[0]->typesys);
		}
		else
	//>> Exp: Exp STAR Exp
		if (N==3 && strcmp(t->child[1]->name, "STAR")==0)
		{
			if (!(t->child[0]->typesys->kind==BASIC 
				&& typeCMP(t->child[0]->typesys, t->child[2]->typesys)==true))
			{
				char buf[128];
				sprintf(buf, "Conflictin types of %s and %s", t->child[0]->data, t->child[2]->data);
				push(&ErrorStack, buf, t->child[0]->line, 7);
			}
			TypeAssign(&(t->typesys), t->child[0]->typesys);
		}
		else
	//>> Exp: Exp DIV Exp
		if (N==3 && strcmp(t->child[1]->name, "DIV")==0)
		{
			if (!(t->child[0]->typesys->kind==BASIC 
				&& typeCMP(t->child[0]->typesys, t->child[2]->typesys)==true))
			{
				char buf[128];
				sprintf(buf, "Conflictin types of %s and %s", t->child[0]->data, t->child[2]->data);
				push(&ErrorStack, buf, t->child[0]->line, 7);
			}
			TypeAssign(&(t->typesys), t->child[0]->typesys);
		}
		else
	//>> Exp: LP Exp RP
		if (N==3 && strcmp(t->child[0]->name, "LP")==0 && strcmp(t->child[2]->name, "RP")==0)
		{
			TypeAssign(&(t->typesys), t->child[1]->typesys);
		}
		else
	//>> Exp: MINUS Exp
		if (N==2 && strcmp(t->child[0]->name, "MINUS")==0)
		{
			if (!(t->child[1]->typesys->kind==BASIC))
			{
				char buf[128];
				sprintf(buf, "Type error using MINUS with %s", t->child[1]->data);
				push(&ErrorStack, buf, t->child[0]->line, 7);
			}
			TypeAssign(&(t->typesys), t->child[1]->typesys);
		}
		else
	//>> Exp: NOT Exp
		if (N==2 && strcmp(t->child[0]->name, "NOT")==0)
		{
			if (!(t->child[1]->typesys->kind==BASIC && t->child[1]->typesys->u.basic==INT_TYPE))
			{
				char buf[128];
				sprintf(buf, "Type error using MINUS with %s", t->child[1]->data);
				push(&ErrorStack, buf, t->child[0]->line, 7);
			}
			TypeAssign(&(t->typesys), t->child[1]->typesys);
		}
		else
	//>> Exp: ID LP Args RP
		if (N==4 && strcmp(t->child[0]->name, "ID")==0)			/* function call */
		{
			/*if ()
			{

			}*/
			TypeAssign(&(t->typesys), t->child[0]->typesys->u.function.ret);
		}
		else
	//>> Exp: ID LP RP
		if (N==3 && strcmp(t->child[0]->name, "ID")==0)			/* function call */
		{

		}
		else
	//>> Exp: Exp LB Exp RB
		if (N==4 && strcmp(t->child[1]->name, "LB")==0)			/* array access */
		{

		}
		else
	//>> Exp: Exp DOT ID
		if (N==3 && strcmp(t->child[1]->name, "DOT")==0)			/* struct access */
		{

		}
		else
	//>> Exp: ID
		if (N==1 && strcmp(t->child[0]->name, "ID")==0)			
		{

		}
		else
	//>> Exp: INT
		if (N==1 && strcmp(t->child[0]->name, "INT")==0)
		{

		}
		else
	//>> Exp: FLOAT
		if (N==1 && strcmp(t->child[0]->name, "FLOAT")==0)
		{

		}
		else 		/* This situation should never appear! */
		{
			printf("You are fucking kidding!\n");
		}
	}
	else if (strcmp(t->name, "Args")==0)
	{
	//>> Args: Exp COMMA Args
	
	//>> Args: Exp
	
	}
	else	// ** Error Message and Abort 
	{
		printf("Error in dfvisit! line :%d\n", __LINE__);
		exit(0);
	}

 

/*
 *	Here begins my Synthetic Attributes' Calculations
 *	for each child. bottom up method.
 */
 	for (; i < N; i++)
 	{
 		dfvisit(t->child[i]);
 	}
	

}

/*
void SaveError(bool judge, int error_no, int line_no)//, char* s)
{
	if (judge==false)
	{
		return;
	}
	else
	{		
		char buf[128];
		switch (error_no)
		{


			case 7:
				sprintf(buf, "Type error using MINUS with %s", //t->child[1]->data);
			break;
		
		}
		
		
		push(&ErrorStack, buf, line_no, 7);
	}

}
*/




#ifdef TEST_DFVISIT

int main()
{


	return 0;
}
#endif
