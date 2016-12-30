#include "head.h"
#ifndef HEAD_H


/*
**


**	x := y 							ASSIGNIC
**	x := y + z 						ADDIC
**	x := y - z						SUBIC
**	x := y * z						MULIC
**	x := y / z						DIVIC

**	DEC x [size] 					DECIC
**	x := &y 						ADDRIC
**	x := *y 						PTRRDIC
**	*x:= y 							PTRWTIC		

**	LABEL x : 						LABELIC
**	GOTO x 							GOTOIC
**	IF x > y GOTO z 				CONDGIC
**	IF x = y GOTO z					CONDEIC
**	IF x < y GOTO z					CONDLIC
**	IF x >= y GOTO z				CONDGEIC
**	IF x <= y GOTO z				CONDLEIC
**	IF x != y GOTO z				CONDNEIC

**	FUNCTION f : 					FUNDEFIC
**	RETURN x 						RETIC
**	ARG x							ARGIC
**	x := CALL f 					CALLIC
**	PARAM x 						PARAMIC

**	READ x 							READIC
**	WRITE x 						WRITEIC
**
*/










enum opKind {
	VARIABLE, CONSTANT, TEMPADDR, LABELJMP, FUNCADDR, DECSIZE
};


typedef struct Operand_
{
	int kind;	/* opKind */
	
	int value; 	/*
				*	Variable:	v1
				*	Constant:	#2
				*	Tempaddr:	t3
				*	Labeljmp:	label4
				*	Funcaddr:	main(5)
				*/
} *Operand;
/* 
* Note as extension to floating point numbers, 
* we can just add a "union" to load "int value" and "float fvalue"
*/

enum insKind {	/* suffix "IC" denotes "Intermediate Code" */
	LABELIC, FUNDEFIC, ASSIGNIC, ADDIC, SUBIC, MULIC, DIVIC, ADDRIC, PTRRDIC, PTRWTIC, GOTOIC,
	CONDGIC, CONDEIC, CONDLIC, CONDGEIC, CONDLEIC, CONDNEIC, 
	RETIC, DECIC, ARGIC, CALLIC, PARAMIC, READIC, WRITEIC,
};

int getOpNum[] = {1, 1, 2, 3, 3, 3, 3, 2, 2, 2, 1,
				  3, 3, 3,
				  1, 2, 1, 2, 1, 1, 1};


typedef struct Code_
{
	int kind;	/* insKind */
	Operand op[3];
	//int op_num  =  getOpNum[kind]
	char TAC[DATALEN];
	struct Code_* next;
	struct Code_* prev;

} *Code;

typedef struct Arglist
{
	int type;
	int value;
	struct Arglist* next, prev;

} *Arglist;



#define  CodeSize sizeof(struct Code_) 




#endif


extern int seed_t;
extern int seed_v;
extern int seed_label;
extern pht SymbolTable;


Code join(Code c1, Code c2)
{
	/* Only Address Assignments */
	//c1->next = c2;	
	//c2->prev = c1;
	if (c1==NULL)
		return c2;
	if (c2==NULL)
		return c1;

	Code c1_tail, c2_head;

	c1_tail = c1;
	while (c1_tail->next != NULL)
		c1_tail = c1_tail->next;
	c1_tail->next = c2;

	c2_head = c2;


	c1_tail->next = c2_head;
	c2_head->prev = c1_tail;

	return c1;
}

int new_temp()
{
	return ++seed_t;			// t1, t2, t3, ...
}

int new_label()
{
	return ++seed_label;		// label1, label2, label3, ...
}

int getVarNo(char* name)
{
	pterm p = findTermByName(SymbolTable, name);		// v1, v2, v3, ...
	if (p==NULL)
	{
		printf("Not Found in SymbolTable, let alone var_no!  function: getVarNo\n");
	}
	return p->var_no;
}

Code gen(int kind, int op_num, ...)		//e.g.   gen(ASSIGNIC, 2, VARIABLE, getVarNo(name), CONSTANT, atoi(t->data))
{	
	va_list arg;
	va_start(arg, op_num);
	
	int i;
	
	Code c = (Code) calloc(1, CodeSize);
	c->kind = kind;	//op_code: ASSIGNIC, GOTOIC, ...
	
	for (i = 0; i < op_num; i++)
	{
		c->op[i] = (Operand) calloc(1, sizeof(struct Operand_));

		/* operands assignments */
		c->op[i]->kind = va_arg(arg, int);	//Variable, Tempaddr, Constant or Labeljmp?
		c->op[i]->value = va_arg(arg, int);	//the uniform value, represented as a integer (v1, t2, #3, label4)
	}

	va_end(arg);

	return c;
}


Code translate_Exp(pnode Exp, int place_kind, int place)
{
	Code c = NULL;
	pnode t = Exp;
	int N = t->childnum;
	if (strcmp(Exp->name, "Exp")!=0)
	{
		printf("Error using translate_Exp! line %d\n", __LINE__);
	}

	//>> Exp: Exp ASSIGNOP Exp
		if (N==3 && strcmp(t->child[1]->name, "ASSIGNOP")==0)
		{
			if (t->child[0]!=NULL && t->child[0]->child[0]!=NULL && strcmp(t->child[0]->child[0]->name, "ID")!=0)
			{
				perror("Exp has to be an ID!");
			}//@ensures Exp1-->ID
			
			pnode Exp1 = t->child[0];
			pnode Exp2 = t->child[2];
			int t1 = new_temp();
			Code c1 = translate_Exp(Exp2, TEMPADDR, t1);
			Code c2 = join(gen(ASSIGNIC, 2, VARIABLE, getVarNo(t->child[0]->child[0]->data), TEMPADDR, t1),
						   place_kind<0 ? NULL : gen(ASSIGNIC, 2, place_kind, place, VARIABLE, getVarNo(t->child[0]->child[0]->data)));

			return join(c1, c2);
		}
		else
	//>> Exp: Exp AND Exp
		if (N==3 && strcmp(t->child[1]->name, "AND")==0)
		{
			pnode Exp1 = t->child[0];
			pnode Exp2 = t->child[2];
		
			int label1 = new_label();
			int label2 = new_label();
		
			Code c0 = place_kind<0 ? NULL : gen(ASSIGNIC, 2, place_kind, place, CONSTANT, 0);
			Code c1 = translate_Cond(Exp, label1, label2);
			Code c2 = join(gen(LABELIC, 1, LABELJMP, label1),
						   place_kind<0 ? NULL : gen(ASSIGNIC, 2, place_kind, place, CONSTANT, 1));
				
		
			return join(join(c0, join(c1, c2)), gen(LABELIC, 1, LABELJMP, label2));
		}
		else
	//>> Exp: Exp OR Exp
		if (N==3 && strcmp(t->child[1]->name, "OR")==0)
		{
			pnode Exp1 = t->child[0];
			pnode Exp2 = t->child[2];
		
			int label1 = new_label();
			int label2 = new_label();
		
			Code c0 = place_kind<0 ? NULL : gen(ASSIGNIC, 2, place_kind, place, CONSTANT, 0);
			Code c1 = translate_Cond(Exp, label1, label2);
			Code c2 = join(gen(LABELIC, 1, LABELJMP, label1),
						   place_kind<0 ? NULL : gen(ASSIGNIC, 2, place_kind, place, CONSTANT, 1));
				
		
			return join(join(c0, join(c1, c2)), gen(LABELIC, 1, LABELJMP, label2));
		}
		else
	//>> Exp: Exp RELOP Exp
		if (N==3 && strcmp(t->child[1]->name, "RELOP")==0)
		{
			pnode Exp1 = t->child[0];
			pnode Exp2 = t->child[2];
		
			int label1 = new_label();
			int label2 = new_label();
		
			Code c0 = gen(ASSIGNIC, 2, place_kind, place, CONSTANT, 0);
			Code c1 = translate_Cond(Exp, label1, label2);
			Code c2 = join(gen(LABELIC, 1, LABELJMP, label1),
						   gen(ASSIGNIC, 2, place_kind, place, CONSTANT, 1));
				
		
			return join(join(c0, join(c1, c2)), gen(LABELIC, 1, LABELJMP, label2));
		}
		else
	//>> Exp: Exp PLUS Exp
		if (N==3 && strcmp(t->child[1]->name, "PLUS")==0)
		{
			pnode Exp1 = t->child[0];
			pnode Exp2 = t->child[2];
			int t1 = new_temp();
			int t2 = new_temp();
			Code c1 = translate_Exp(Exp1, TEMPADDR, t1);
			Code c2 = translate_Exp(Exp2, TEMPADDR, t2);
			Code c3 = place_kind<0 ? NULL : gen(ADDIC, 3, place_kind, place, TEMPADDR, t1, TEMPADDR, t2);
			return join(c1, join(c2, c3));	
		}
		else
	//>> Exp: Exp MINUS Exp
		if (N==3 && strcmp(t->child[1]->name, "MINUS")==0)
		{
			pnode Exp1 = t->child[0];
			pnode Exp2 = t->child[2];
			int t1 = new_temp();
			int t2 = new_temp();
			Code c1 = translate_Exp(Exp1, TEMPADDR, t1);
			Code c2 = translate_Exp(Exp2, TEMPADDR, t2);
			Code c3 = place_kind<0 ? NULL : gen(SUBIC, 3, place_kind, place, TEMPADDR, t1, TEMPADDR, t2);
			return join(c1, join(c2, c3));	
		}
		else
	//>> Exp: Exp STAR Exp
		if (N==3 && strcmp(t->child[1]->name, "STAR")==0)
		{
			pnode Exp1 = t->child[0];
			pnode Exp2 = t->child[2];
			int t1 = new_temp();
			int t2 = new_temp();
			Code c1 = translate_Exp(Exp1, TEMPADDR, t1);
			Code c2 = translate_Exp(Exp2, TEMPADDR, t2);
			Code c3 = place_kind<0 ? NULL : gen(MULIC, 3, place_kind, place, TEMPADDR, t1, TEMPADDR, t2);
			return join(c1, join(c2, c3));	
		}
		else
	//>> Exp: Exp DIV Exp
		if (N==3 && strcmp(t->child[1]->name, "DIV")==0)
		{
			pnode Exp1 = t->child[0];
			pnode Exp2 = t->child[2];
			int t1 = new_temp();
			int t2 = new_temp();
			Code c1 = translate_Exp(Exp1, TEMPADDR, t1);
			Code c2 = translate_Exp(Exp2, TEMPADDR, t2);
			Code c3 = place_kind<0 ? NULL : gen(DIVIC, 3, place_kind, place, TEMPADDR, t1, TEMPADDR, t2);
			return join(c1, join(c2, c3));
		}
		else
	//>> Exp: LP Exp RP
		if (N==3 && strcmp(t->child[0]->name, "LP")==0 
				&& strcmp(t->child[2]->name, "RP")==0)
		{
			return translate_Exp(t->child[1], place_kind, place);	// doubt
		}
		else
	//>> Exp: MINUS Exp1
		if (N==2 && strcmp(t->child[0]->name, "MINUS")==0)
		{
			pnode Exp1 = t->child[1];
			Code c;
			if (Exp1->child[0]!=NULL && strcmp(Exp1->child[0]->name, "INT")==0)
			{
				int Constant = - atoi(Exp1->child[0]->data);
				c = place_kind<0 ? NULL : gen(ASSIGNIC, 2, place_kind, place, CONSTANT, Constant);
			}
			else
			{
				int t1 = new_temp();
				Code c1 = translate_Exp(Exp1, TEMPADDR, t1);
				Code c2 = place_kind<0 ? NULL : gen(SUBIC, 3, place_kind, place, CONSTANT, 0, TEMPADDR, t1);	
				c = join(c1, c2);
			}
			
			return c;
		}
		else
	//>> Exp: NOT Exp
		if (N==2 && strcmp(t->child[0]->name, "NOT")==0)
		{
			pnode Exp1 = t->child[1];
			
		
			int label1 = new_label();
			int label2 = new_label();
		
			Code c0 = place_kind<0 ? NULL : gen(ASSIGNIC, 2, place_kind, place, CONSTANT, 0);
			Code c1 = translate_Cond(Exp, label1, label2);
			Code c2 = join(gen(LABELIC, 1, LABELJMP, label1),
						   gen(ASSIGNIC, 2, place_kind, place, CONSTANT, 1));
				
		
			return join(join(c0, join(c1, c2)), gen(LABELIC, 1, LABELJMP, label2));
		}
		else
	//>> Exp: ID LP Args RP
		if (N==4 && strcmp(t->child[0]->name, "ID")==0)			/* function call */
		{
			int i;
			int Num;
			char* f_name = t->child[0]->data;
			int val = getVarNo(f_name);

			Arglist list = NULL;
			Code c1 = translate_Args(t->child[2], &list);
			
			if (strcmp(f_name, "write")==0)
				return join(c1, gen(WRITEIC, 1, list->type, list->value));
			
			Code c2 = gen(ARGIC, 1, list->type, list->value);
			
			while (list->next != NULL)
			{
				list = list->next;
				c2 = join(c2, gen(ARGIC, 1, list->type, list->value));
			}

			return join(c1, join(c2, gen(CALLIC, 2, place_kind, place, FUNCADDR, val)));
		}
		else
	//>> Exp: ID LP RP
		if (N==3 && strcmp(t->child[0]->name, "ID")==0)			/* function call */
		{
			char* f_name = t->child[0]->data;
			int val = getVarNo(f_name);
			if (strcmp(f_name, "read")==0)
			{
				return gen(READIC, 1, place_kind, place);
			}
			return gen(CALLIC, 2, place_kind, place, FUNCADDR, val);

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
			int v1 = getVarNo(t->child[0]->data);

			return place_kind<0 ? NULL : gen(ASSIGNIC, 2, place_kind, place, VARIABLE, v1);
		}
		else
	//>> Exp: INT
		if (N==1 && strcmp(t->child[0]->name, "INT")==0)
		{
			
			int value = atoi(t->child[0]->data);
			
			return place_kind<0 ? NULL : gen(ASSIGNIC, 2, place_kind, place, CONSTANT, value);
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





	return c;
}

Code translate_Args(pnode t, Arglist* lst)
{
	if (strcmp(t->name, "Args")!=0)
	{
		printf("Error using translate_Args! line %d\n", __LINE__);
	}
	pnode Args = t;
	int N = t->childnum;
	pnode Exp = t->child[0];

	//>> Args: Exp
	if (N==1)
	{
		
		int t1 = new_temp();
		Code c1 = translate_Exp(Exp, TEMPADDR, t1);
		
		/* write into new_arg */
		Arglist new_arg = (Arglist) calloc(1, sizeof(struct Arglist_));
		new_arg->type = TEMPADDR;
		new_arg->value = t1;
		
		/* push new_arg */
		new_arg->next = *lst;
		*lst = new_arg;
		
		return c1;
	}
	//>> Args: Exp COMMA Args1
	else
	{
		pnode Args1 = t->child[2];
	
		int t1 = new_temp();
		Code c1 = translate_Exp(Exp, TEMPADDR, t1);
		
		/* write into new_arg */
		Arglist new_arg = (Arglist) calloc(1, sizeof(struct Arglist_));
		new_arg->type = TEMPADDR;
		new_arg->value = t1;

		/* push new_arg */
		new_arg->next = *lst;
		*lst = new_arg;

		Code c2 = translate_Args(Args1, lst);

		return join(c1, c2);
	}



}




Code translate_Stmt(pnode Stmt)
{
	pnode t = Stmt;
	int N = t->childnum;
	if (strcmp(t->name, "Stmt")!=0)
	{
		printf("Error using translate_Exp! line %d\n", __LINE__);
	}

	#ifdef DEBUG_MODE
	printf("Translating %s in function translate_Stmt\n", t->name);
	#endif
	
	//>> Stmt: Exp SEMI
	if (N==2 && strcmp(t->child[0]->name, "Exp")==0)
	{
		return translate_Exp(t->child[0], -1, -1);	
	}
	//>> Stmt: CompSt
	else if (N==1 && strcmp(t->child[0]->name, "CompSt")==0)
	{
		return translate_CompSt(t->child[0]);	//TODO
	}
	//>> Stmt: RETURN Exp SEMI
	else if (N==3 && strcmp(t->child[0]->name, "RETURN")==0)
	{
		pnode Exp = t->child[1];
		/*if (strcmp(Exp->child[0]->name, "INT")==0)
		{
			return gen(RETIC, 1, CONSTANT, atoi(Exp->child[0]->data));
		}
		else if (strcmp(Exp->child[0]->name, "ID")==0)
		{
			return gen(RETIC, 1, VARIABLE, getVarNo(Exp->child[0]->data));
		}
		else
		{*/
			int t1 = new_temp();
			Code c1 = translate_Exp(Exp, TEMPADDR, t1);
			Code c2 = gen(RETIC, 1, TEMPADDR, t1);
			return join(c1, c2);	
		//}
		
	}
	//>> Stmt: IF LP Exp RP Stmt1
	else if (N==5 && strcmp(t->child[0]->name, "IF")==0)
	{
		
		pnode Exp = t->child[2];
		pnode Stmt1 = t->child[4];
		int label1 = new_label();
		int label2 = new_label();
		Code c1 = translate_Cond(Exp, label1, label2);
		Code c2 = translate_Stmt(Stmt1);

		return join(join(c1, gen(LABELIC, 1, LABELJMP, label1)), join(c2, gen(LABELIC, 1, LABELJMP, label2)));
	}
	//>> Stmt: IF LP Exp RP Stmt ELSE Stmt
	else if (N==7 && strcmp(t->child[0]->name, "IF")==0)
	{
		pnode Exp = t->child[2];
		pnode Stmt1 = t->child[4];
		pnode Stmt2 = t->child[6];
		int label1 = new_label();
		int label2 = new_label();
		int label3 = new_label();
		Code c1 = translate_Cond(Exp, label1, label2);
		Code c2 = translate_Stmt(Stmt1);
		Code c3 = translate_Stmt(Stmt2);

		Code s = join(c1, gen(LABELIC, 1, LABELJMP, label1));
		s = join(s, c2);
		s = join(s, gen(GOTOIC, 1, LABELJMP, label3));
		s = join(s, gen(LABELIC, 1, LABELJMP, label2));
		s = join(s, c3);
		s = join(s, gen(LABELIC, 1, LABELJMP, label3));
		
		return s;
			
	}
	//>> Stmt: WHILE LP Exp RP Stmt
	else 
	{
		pnode Exp = t->child[2];
		pnode Stmt1 = t->child[4];
		int label1 = new_label();
		int label2 = new_label();
		int label3 = new_label();
		Code c1 = translate_Cond(Exp, label2, label3);
		Code c2 = translate_Stmt(Stmt1);
		Code s = gen(LABELIC, 1, LABELJMP, label1);
		s = join(s, c1);
		s = join(s, gen(LABELIC, 1, LABELJMP, label2));
		s = join(s, c2);
		s = join(s, gen(GOTOIC, 1, LABELJMP, label1));
		s = join(s, gen(LABELIC, 1, LABELJMP, label3));
		
		return s;

	}

}




Code translate_Cond(pnode Exp, int label_true, int label_false)
{
	if (strcmp(Exp->name, "Exp")!=0)
	{
		printf("Error using translate_Cond! line %d\n", __LINE__);
	}
	pnode t = Exp;
	int N = t->childnum;

	#ifdef DEBUG_MODE
	printf("Translating %s in function translate_Cond\n", t->name);
	#endif
	//>> Exp: Exp1 RELOP Exp2
		if (N==3 && strcmp(t->child[1]->name, "RELOP")==0)
		{
			pnode Exp1 = t->child[0];
			pnode Exp2 = t->child[2];
			int t1 = new_temp();
			int t2 = new_temp();
			Code c1 = translate_Exp(Exp1, TEMPADDR, t1);
			Code c2 = translate_Exp(Exp2, TEMPADDR, t2);
			int Relop; char* s = t->child[1]->data;

			if (strcmp(s, ">")==0)
			{
				Relop = CONDGIC;
			}
			else if (strcmp(s, "<")==0)
			{
				Relop = CONDLIC;
			}
			else if (strcmp(s, "==")==0)
			{
				Relop = CONDEIC;
			}
			else if (strcmp(s, ">=")==0)
			{
				Relop = CONDGEIC;
			}
			else if (strcmp(s, "<=")==0)
			{
				Relop = CONDLEIC;
			}
			else 			//  !=
			{
				Relop = CONDNEIC;
			}

			Code c3 = gen(Relop, 3, TEMPADDR, t1, TEMPADDR, t2, LABELJMP, label_true);

			Code c = join(c1, c2);
			c = join(c, c3);
			c = join(c, gen(GOTOIC, 1, LABELJMP, label_false));
			return c;
		}
	//>> Exp: NOT Exp1	
		else if (N==2 && strcmp(t->child[0]->name, "NOT")==0)
		{
			pnode Exp1 = t->child[1];

			return translate_Cond(Exp1, label_false, label_true);

		}
	//>> Exp: Exp1 AND Exp2
		else if (N==3 && strcmp(t->child[1]->name, "AND")==0)
		{
			pnode Exp1 = t->child[0];
			pnode Exp2 = t->child[2];
			
			int label1 = new_label();
			Code c1 = translate_Cond(Exp1, label1, label_false);
			Code c2 = translate_Cond(Exp2, label_true, label_false);

			return join(c1, join(gen(LABELIC, 1, LABELJMP, label1), c2));
		}
	//>> Exp: Exp1 OR Exp2
		else if (N==3 && strcmp(t->child[1]->name, "OR")==0)
		{
			pnode Exp1 = t->child[0];
			pnode Exp2 = t->child[2];
			
			int label1 = new_label();
			Code c1 = translate_Cond(Exp1, label_true, label1);
			Code c2 = translate_Cond(Exp2, label_true, label_false);

			return join(c1, join(gen(LABELIC, 1, LABELJMP, label1), c2));
		}
		else
		{
			int t1 = new_temp();
			Code c1 = translate_Exp(Exp, TEMPADDR, t1);
			Code c2 = gen(CONDNEIC, 3, TEMPADDR, t1, CONSTANT, 0, LABELJMP, label_true);
			return join(c1, join(c2, gen(GOTOIC, 1, LABELJMP, label_false)));
		}
	
	
}



Code translate_CompSt(pnode CompSt)
{
	if (strcmp(CompSt->name, "CompSt")!=0)
	{
		printf("Error using translate_CompSt! line %d\n", __LINE__);
	}
	//>> CompSt: LC DefList StmtList RC
	pnode DefList = CompSt->child[1];
	pnode StmtList = CompSt->child[2];

	return join(translate(DefList), translate(StmtList));
}












int getWidth(Type typesys)
{
	int width = 0;
	FieldList ptr = NULL;
	
	if (typesys==NULL)
		perror("Error width computing with NULL typesys!\n");
	
	if (typesys->kind==STRUCTURE || typesys->kind==OBJECT )
		printf("Structure and Objects not supported currently\n");

	switch (typesys->kind)
	{
		case BASIC:
			width = 4;
		break;
		
		case ARRAY:
			width = typesys->u.array.size * getWidth(typesys->u.array.elem);
		break;
		
		case STRUCTURE:	/* Not sure about correctness */
			ptr = typesys->u.structure;
			while(ptr!=NULL)
			{
				width += getWidth(ptr->type);
				ptr = ptr->next;
			}
		break;
		
		case OBJECT:
			/* I don't know */
		break;
		
		case FUNCTION:
			/* Maybe 0 bytes */
		break;

		default:
			printf("Not intialized typesys, error compututation\n");
	}


	return width;
}










































Code translate(pnode t)
{
	if (t==NULL)	//For empty nodes or leaf nodes, just return 
		return NULL;

	#ifdef DEBUG_MODE
	printf("Translating %s\n", t->name);
	#endif
	

	int i = 0;
	int N = t->childnum;


	/********************	High-level Definitions *****************/
	if (strcmp(t->name, "Program")==0)
	{
	//>> Program: ExtDefList
		return translate(t->child[0]);
	}
	else if (strcmp(t->name, "ExtDefList")==0)
	{
	//>> ExtDefList: ExtDef ExtDefList
		if (N==2)
		{
			return join(translate(t->child[0]), translate(t->child[1]));	
		}
	//>> ExtDefList: eps
		else 
		{
			return NULL;
		}
	}
	else if (strcmp(t->name, "ExtDef")==0)
	{																			
	//>> ExtDef: Specifier ExtDecList SEMI
		if (N==3 && strcmp(t->child[1]->name, "ExtDecList")==0)
		{
			return translate(t->child[1]);
		}
	//## ExtDef: Specifier FunDec CompSt
		else if (N==3 && strcmp(t->child[1]->name, "FunDec")==0)
		{
			return join(translate(t->child[1]), translate_CompSt(t->child[2]));
		}
	//>> ExtDef: Specifier SEMI
		else
		{
			return NULL;	//Never get in
		}
	
	}
	else if (strcmp(t->name, "ExtDecList")==0)
	{
	//>> ExtDecList: VarDec
		if (N==1)
		{
			return translate(t->child[0]);
		}
	//>> ExtDecList: VarDec COMMA ExtDecList
		else
		{
			return join(translate(t->child[0]), translate(t->child[2]));
		}
			
	}
	/********************   Specifier    **********************/
	else if (strcmp(t->name, "Specifier")==0)
	{
			return NULL;
	//>> Specifier: TYPE
		
	//>> Specifier: StructSpecifier
	
	}
	else if (strcmp(t->name, "StructSpecifier")==0)
	{	
			return NULL;
	//>> StructSpecifier: STRUCT OptTag LC DefList RC
	
	//>> StructSpecifier: STRUCT Tag
	
	}
	else if (strcmp(t->name, "OptTag")==0)
	{
			return NULL;
	//>> OptTag: ID
	
	//>> OptTag: eps
	}
	else if (strcmp(t->name, "Tag")==0)
	{	
			return NULL;
	//>> Tag: ID
	
	}
	/*******************    Declarators   *********************/
	else if (strcmp(t->name, "VarDec")==0)
	{
	//>> VarDec: ID
		if (N==1)
		{
			/* TODO: need checking */
			pterm q = findTermByName(SymbolTable, t->child[0]->data);
			q->var_no = ++seed_v;

			//int t1 = new_temp();
			//Code c1 = gen(ASSIGNIC, 2, TEMPADDR, t1, VARIABLE, q->var_no);
			return NULL;
		}
	//>> VarDec: VarDec LB INT RB
		else
		{
			/*TODO: Array*/
		}
	}
	else if (strcmp(t->name, "FunDec")==0)
	{
	//>> FunDec: ID LP VarList RP
		if (N==4)
		{//TODO : need checking
			Code c1 = gen(FUNDEFIC, 1, FUNCADDR, getVarNo(t->child[0]->data));
			Code c2 = translate(t->child[2]);
			return join(c1, c2);
		}
	//>> FunDec: ID LP RP
		else
		{
			return gen(FUNDEFIC, 1, FUNCADDR, getVarNo(t->child[0]->data));
		}
	}
	else if (strcmp(t->name, "VarList")==0)
	{
	//>> VarList: ParamDec COMMA VarList
		if (N==3)
		{
			return join(translate(t->child[0]), translate(t->child[2]));
		}
	//>> VarList: ParamDec
		else
		{
			return translate(t->child[0]);	//TODO : need checking
		}
	}
	else if (strcmp(t->name, "ParamDec")==0)
	{
	//>> ParamDec: Specifier VarDec
		Code c1 = translate(t->child[1]);	//c1 must be a NULL
		if (c1!=NULL)
			printf("Warning: c1!=NULL! line %d\n", __LINE__);
		return gen(PARAMIC, 1, VARIABLE, getVarNo(t->child[1]->data));

	}	
	
	/*******************   Statements   ************************/
	else if (strcmp(t->name, "CompSt")==0)
	{//##
	//>> CompSt: LC DefList StmtList RC
		return translate_CompSt(t);
	}	
	else if (strcmp(t->name, "StmtList")==0)
	{//##
	//>> StmtList: Stmt StmtList
		if (N==2)
		{
			Code c1 = translate_Stmt(t->child[0]);
			Code c2 = translate(t->child[1]);
			return join(c1, c2);
		}
	//>> StmtList: eps
		else
		{
			return NULL;
		}

	}	
	else if (strcmp(t->name, "Stmt")==0)
	{//##
		return translate_Stmt(t);
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
		{
			return join(translate(t->child[0]), translate(t->child[1]));	
		}
	//>> DefList: eps
		else
		{
			return NULL;
		}
	}
	else if (strcmp(t->name, "Def")==0)
	{
	//>> Def: Specifier DecList SEMI
		return translate(t->child[1]);
	}
	else if (strcmp(t->name, "DecList")==0)
	{
	//>> DecList: Dec
		if (N==1)
		{
			return translate(t->child[0]);
		}
	//>> DecList: Dec COMMA DecList
		else
		{
			return join(translate(t->child[0]), translate(t->child[2]));
		}
	}
	else if (strcmp(t->name, "Dec")==0)
	{
	//>> Dec: VarDec
		if (N==1)
		{
			return translate(t->child[0]);
		}
	//>> Dec: VarDec ASSIGNOP Exp
		else
		{	//TODO: need checking
			int t1 = new_temp();
			Code c1 = translate_Exp(t->child[2], TEMPADDR, t1);
			Code c2 = translate(t->child[0]);
			return join(c1, join(c2, gen(ASSIGNIC, 2, VARIABLE, getVarNo(t->child[0]->data), TEMPADDR, t1)));
		}
	}
	/************************  Exp  *****************************/
	else if (strcmp(t->name, "Exp")==0)
	{//##
		//TODO : need checking
		int t1 = new_temp();

		return translate_Exp(t, TEMPADDR, t1);
	}
	else if (strcmp(t->name, "Args")==0)
	{//##
		
		return NULL;	/* Never get in */
	//>> Args: Exp COMMA Args
	
	//>> Args: Exp
	
	}
	else	// ** Error Message and Abort 
	{
		printf("Error in dfvisit! line :%d\n", __LINE__);
		exit(0);
	}

 

/*
*	the recursive computing for the remainder
*/
 	
	
 	return NULL;
}




void setFunctionVarNo()
{
	
	Type tp = (Type) calloc(1, sizeof(struct Type_));
	tp->kind = FUNCTION;
	insertHT(SymbolTable, "read", tp);
	insertHT(SymbolTable, "write", tp);
	
	int i;
	for (i = 0; i < SymbolTable->N; i++)
	{
		pterm ptr = SymbolTable->a[i];
		while (ptr!=NULL)
		{
			if (ptr->typesys->kind==FUNCTION)
			{
				ptr->var_no = i+1;	
			}
			ptr = ptr->next;
		}
	}

}
		
char* getFunctionNameByVarNo(int var_no)
{
	char* s = NULL;
	pterm ptr;
	int i;

	for (i = 0; i < SymbolTable->N; i++)
	{
		ptr = SymbolTable->a[i];
		while (ptr!=NULL)
		{
			if (ptr->typesys->kind==FUNCTION && ptr->var_no==var_no)
			{
				s = ptr->name;
			}
			ptr = ptr->next;
		}
	}

	return s;
}






char* toString(Operand op)
{
	char* s = (char *) calloc(LEN, sizeof(char));
	if (op==NULL)
		return NULL;
	
	switch (op->kind)
	{
		case VARIABLE:
		sprintf(s, "v%d", op->value);
		break;

		case CONSTANT:
		sprintf(s, "#%d", op->value);
		break;
		
		case TEMPADDR:
		sprintf(s, "t%d", op->value);
		break;
		
		case LABELJMP:
		sprintf(s, "label%d", op->value);
		break;

		case FUNCADDR:
		sprintf(s, "%s", getFunctionNameByVarNo(op->value));
		break;

		case DECSIZE:
		sprintf(s, "%d", op->value);
		break;
	}	
	
	return s;	
}



void printIC(Code line)
{
	int i;
	char* opstr[3];
	FILE* f = fopen(".//IC.txt", "w+");

	while (line!=NULL)
	{
		for (i = 0; i < 3; i++)
		{
			opstr[i] = toString(line->op[i]);
		}

		switch (line->kind)
		{
			case ASSIGNIC:
			{
				sprintf(line->TAC, "%s := %s", opstr[0], opstr[1]);	
			}
			break;
			
			case ADDIC:
			{
				sprintf(line->TAC, "%s := %s + %s", opstr[0], opstr[1], opstr[2]);
			}
			break;
			
			case SUBIC:
			{
				sprintf(line->TAC, "%s := %s - %s", opstr[0], opstr[1], opstr[2]);
			}
			break;
			
			case MULIC:
			{
				sprintf(line->TAC, "%s := %s * %s", opstr[0], opstr[1], opstr[2]);
			}
			break;
			
			case DIVIC:
			{
				sprintf(line->TAC, "%s := %s / %s", opstr[0], opstr[1], opstr[2]);
			}
			break;

 			case DECIC:
 			{
 				sprintf(line->TAC, "DEC %s %s", opstr[0], opstr[1]);
 			}
 			break;

			case ADDRIC:
			{
				sprintf(line->TAC, "%s := &%s", opstr[0], opstr[1]);
			}
			break;

 			case PTRRDIC:
 			{
 				sprintf(line->TAC, "%s := *%s", opstr[0], opstr[1]);
 			}
 			break;
			
			case PTRWTIC:
			{
				sprintf(line->TAC, "*%s := %s", opstr[0], opstr[1]);
			}
			break;
			
			case LABELIC:
			{
				sprintf(line->TAC, "LABEL %s :", opstr[0]);
			}
			break;
 			
 			case GOTOIC:
 			{
 				sprintf(line->TAC, "GOTO %s", opstr[0]);	
 			}
 			break;

 			case CONDGIC:
 			{
 				sprintf(line->TAC, "IF %s > %s GOTO %s", opstr[0], opstr[1], opstr[2]);
 			}
 			break;

			case CONDEIC:
			{
				sprintf(line->TAC, "IF %s == %s GOTO %s", opstr[0], opstr[1], opstr[2]);
			}
			break;

			case CONDLIC:
			{
				sprintf(line->TAC, "IF %s < %s GOTO %s", opstr[0], opstr[1], opstr[2]);
			}
			break;

			case CONDGEIC:
			{
				sprintf(line->TAC, "IF %s >= %s GOTO %s", opstr[0], opstr[1], opstr[2]);
			}
			break;
			
			case CONDLEIC:
			{
				sprintf(line->TAC, "IF %s <= %s GOTO %s", opstr[0], opstr[1], opstr[2]);
			}
			break;

			case CONDNEIC:
			{
				sprintf(line->TAC, "IF %s != %s GOTO %s", opstr[0], opstr[1], opstr[2]);
			}
			break;

			case FUNDEFIC:
			{
				sprintf(line->TAC, "\nFUNCTION %s :", opstr[0]);
				
			}
			break;

			case RETIC:
			{
				sprintf(line->TAC, "RETURN %s", opstr[0]);
			}
			break;
			
			case ARGIC:
			{
				sprintf(line->TAC, "ARG %s", opstr[0]);
			}
			break;
			
			case CALLIC:
			{
				sprintf(line->TAC, "%s := CALL %s", opstr[0], opstr[1]);
			}
			break;
			
			case PARAMIC:
			{
				sprintf(line->TAC, "PARAM %s", opstr[0]);
			}
			break;

			case READIC:
			{
				sprintf(line->TAC, "READ %s", opstr[0]);
			}
			break;

			case WRITEIC:
			{
				sprintf(line->TAC, "WRITE %s", opstr[0]);
			}
			break;
		}

		fprintf(f, "%s\n", line->TAC);
		printf("%s\n", line->TAC);
		line = line->next;
	}


	fclose(f);
}




































/*













Code optimizeIC(Code line)
{
	int i;
	char* opstr[3];
	

	while (line!=NULL)
	{
		for (i = 0; i < 3; i++)
		{
			opstr[i] = toString(line->op[i]);
		}

		switch (line->kind)
		{
			case ASSIGNIC:
			{
				
			}
			break;
			
			case ADDIC:
			{
				
			}
			break;
			
			case SUBIC:
			{
				
			}
			break;
			
			case MULIC:
			{
				
			}
			break;
			
			case DIVIC:
			{
				
			}
			break;

 			case DECIC:
 			{
 				
 			}
 			break;

			case ADDRIC:
			{
				
			}
			break;

 			case PTRRDIC:
 			{
 				
 			}
 			break;
			
			case PTRWTIC:
			{
				
			}
			break;
			
			case LABELIC:
			{
				Code next_line = line->next;
				if (next_line!=NULL && next_line->kind==LABELIC)
				{
					if (next_line->op[0] > line->op[0])
					{

					}
				}
			}
			break;
 			
 			case GOTOIC:
 			{
 				if ()
 			}
 			break;

 			case CONDGIC:
 			{
 				
 			}
 			break;

			case CONDEIC:
			{
				
			}
			break;

			case CONDLIC:
			{
				
			}
			break;

			case CONDGEIC:
			{
				
			}
			break;
			
			case CONDLEIC:
			{
				
			}
			break;

			case CONDNEIC:
			{
				
			}
			break;

			case FUNDEFIC:
			{
				
				
			}
			break;

			case RETIC:
			{
				
			}
			break;
			
			case ARGIC:
			{
				
			}
			break;
			
			case CALLIC:
			{
				
			}
			break;
			
			case PARAMIC:
			{
				
			}
			break;

			case READIC:
			{
				
			}
			break;

			case WRITEIC:
			{
				
			}
			break;
		}

		fprintf(f, "%s\n", line->TAC);
		printf("%s\n", line->TAC);
		line = line->next;
	}


	fclose(f);
}


*/
