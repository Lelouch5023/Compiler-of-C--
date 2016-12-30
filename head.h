#ifndef HEAD_H
#define HEAD_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#define bool int
#define true 1
#define false 0
/** ARY is the maximum aries of a tree  **/
#define ARY 10
#define LEN 24
#define DATALEN 40
int yyerror(char* s);
int yylex(void);

struct Code_;
typedef struct Code_* Code;

/*
 *		>>>>>>Type System Interface
 *
 */

struct Type_;
struct FieldList_;
enum {BASIC, ARRAY, STRUCTURE, FUNCTION, OBJECT};
enum {INT_TYPE, FLOAT_TYPE};

typedef struct Type_
{
	int kind;	/* BASIC, ARRAY, STRUCTURE, FUNCTION, OBJECT (minus value means disabled) */
	union {
		int basic;	/* INT_TYPE, FLOAT_TYPE */
		struct { struct Type_* elem; int size; } array;
		struct FieldList_* structure;
		struct { struct Type_* ret; struct FieldList_* param; char funcname[DATALEN];} function;//Extension
		struct { char structname[DATALEN]; struct FieldList_* structfield; } object;	//Extention : an object of struct/class
	} u;
} *Type;		// Caution: Object.structfield is just an address!

typedef struct FieldList_
{
	char* name;
	struct Type_* type;
	struct FieldList_* next;
} *FieldList;

bool paramCMP(FieldList A, FieldList B);
bool fieldlistCMP(FieldList A, FieldList B);
bool typeCMP(Type a, Type b);
void fieldlistAssign(FieldList* des, FieldList src);
Type TypeAssign(Type* des, Type src);


/*
 *		Syntax Tree System
 */



enum {	/* Discarded */
	EINT, EFLOAT, ECHAR, ESTRING
};


typedef struct __node 
{
    char name[LEN]; /* name, such as "INT", "FLOAT", "COMMA" */
	int line; /* in which line it appears */

    int type;	/* mark it is a LEAF or NODE:  (Note: if LEAF, data[] is valid; if NODE, data[] is disabled)  */  
    
	union { /* raw data, such as "1", "3.14", "'this is a string'", "true"  */
		char in_data[DATALEN];	//inherit
		char data[DATALEN];		//synthetic
	};
	
	union {	/* data type according to Type System (Note: be it a NODE or LEAF, the typesys is valid ) */
		Type in_typesys;		//inherit
		Type typesys;			//synthetic
	};
	
	int scope;	/* scope  */	

	Code code;	/* S.code */
	Code next;	/* S.next */
	int kind_place;	/* flag: VARIABLE, TEMPADDR, CONSTANT */
	int place;	/* S.place */

	int childnum;
    struct __node* parent;
    struct __node* child[ARY];

}node, *pnode;
#define YYSTYPE pnode

pnode makenode(char* name, int number, ...);
void treeTrav(pnode t);
void syntaxAnalysis(pnode t);
void treePrint(pnode t, int n);

/*
 *
 *		>>>>>> Hash Table System
 *
 */

/*
 *  新表，newHT
 *	插入，insertHT
 *	查找，getHT
 *	删除，delHT
 */
typedef struct __term
{
	/*	data field	*/
	char name[DATALEN];
	Type typesys;
	int scope;
	unsigned addr;
	int var_no;
	/*	ptr field	*/
	struct __term * next;

}term, *pterm;

typedef struct __ht
{
	int N;	//no. of aries
	int total;	//total num of terms
	int* Num;	//num of terms from each branch 
	pterm* a;	//then a[k] is the address of the term

}ht, *pht;

int hashCMP(pterm A, pterm B);
pht newHT(int N);
unsigned hash(char* name, int N);
int insertHT(pht H, char* name, Type typesys);
int getHT(pht H, char* name, Type typesys, int* index, int* forward);
int delHT(pht H, char* name, Type typesys);
Type getTypeByName(pht H, char* name);
pterm findTermByName(pht H, char* name);

/*
 *
 *			>>> Error Info Stack System
 *
 */

typedef struct __Error
{
	char msg[128];
	int line;
	int errortype;
	struct __Error* next;

}Error, *pError;

void push(pError* stack, char* s, int line, int errortype);
void pop(pError stack);
pError newErrStack();


void dfvisit(pnode t);
void printSymbolTable(void);
void getstring(Type tp);



/*
 *
 *			>>> Intermediate Code Generation System
 *
 */




/*
**
**	LABEL x : 						LABELIC
**	FUNCTION f : 					FUNDEFIC
**	x := y 							LABELIC
**	x := y + z 						ADDIC
**	x := y - z						SUBIC
**	x := y * z						MULIC
**	x := y / z						DIVIC
**	x := &y 						ADDRIC
**	x := *y 						PTRRDIC
**	*x:= y 							PTRWTIC		
**	GOTO x 							GOTOIC
**	IF x > y GOTO z 				CONDGIC
**	IF x = y GOTO z					CONDEIC
**	IF x < y GOTO z					CONDLIC
**	IF x >= y GOTO z				CONDGEIC
**	IF x <= y GOTO z				CONDLEIC
**	IF x != y GOTO z				CONDNEIC
**	RETURN x 						RETIC
**	DEC x [size] 					DECIC
**	ARG x							ARGIC
**	x := CALL f 					CALLIC
**	PARAM x 						PARAMIC
**	READ x 							READIC
**	WRITE x 						WRITEIC
**
*/



enum opKind {
	VARIABLE, CONSTANT, TEMPADDR, LABELJMP, FUNCADDR, DECSIZE,
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
				*	Decsize :   8
				*/
} *Operand;
/* Note as extension to floating point numbers, 
* we can just add a "union" to load "int value" and "float fvalue"
*/

enum insKind {	/* suffix "IC" denotes "Intermediate Code" */
	LABELIC, FUNDEFIC, ASSIGNIC, ADDIC, SUBIC, MULIC, DIVIC, ADDRIC, PTRRDIC, PTRWTIC, GOTOIC,
	CONDGIC, CONDEIC, CONDLIC, CONDGEIC, CONDLEIC, CONDNEIC, 
	RETIC, DECIC, ARGIC, CALLIC, PARAMIC, READIC, WRITEIC,
};




typedef struct Code_
{
	int kind;	/* insKind */
	Operand op[3];
	struct Code_* ins;	/* The lines of mips code */

	char TAC[3*DATALEN];	/* For intermediate code in lab3 */
	char MIPS[3*DATALEN];	/* For MIPS code in lab4 */
	
	struct Code_* next;
	struct Code_* prev;

} *Code;


typedef struct Arglist_
{
	int type;
	int value;
	struct Arglist_* next;
	struct Arglist_* prev;

} *Arglist;


int getWidth(Type typesys);

Code join(Code c1, Code c2);
int new_temp();
int new_label();
int getVarNo(char* name);

Code gen(int kind, int op_num, ...);
Code translate_Exp(pnode Exp, int place_kind, int place);
Code translate_Args(pnode t, Arglist* lst);
Code translate_Stmt(pnode Stmt);
Code translate_Cond(pnode Exp, int label_true, int label_false);
Code translate_CompSt(pnode CompSt);

Code translate(pnode t);

void setFunctionVarNo();
char* getFunctionNameByVarNo(int var_no);

char* toString(Operand op);
void printIC(Code line);
#define  CodeSize sizeof(struct Code_) 

/*	
*		
*		Target Code	
*		
*/



enum MIPS_INS {	/* reuse of data structure Code->kind */
	add = 100, addu, sub, subu, and, or, xor, nor, slt, sltu, sll, srl, sra, sllv, srlv, srav, jl,	/* R-type Instruction */
	addi, addiu, andi, ori, xori, lui, lw, sw, beq, bne, slti, sltiu,							/* I-type Instruction */				
	j, jal,																						/* J-type Instruction */
	sle, sgt, sge, mflo,																			// I don't know
	li, la, move, bgt, bge, blt, ble, _label, _fun,													/* Pseudo Instruction */
	mul, divm, 
};


enum MIPS_OPS {
	REGS = 10, 	/* Variable:  $s0, $s1, $s2, $s3, ...., $s7, $t9, $t8,...$t3 */
	REGT, 	//>> special usage as temporary storage
	REGA,	/* $a0, $a1, $a2, $a3 */
	REGV,	/* $v0 */
	REGRA,	/* $ra */
	REGSP,	/* $sp */
	MEM,	/* Tempaddr:  */
	IMME,	//CONSTANT
	LBL,	//LABELJMP
	FUND,   //FUNCADDR
};

void printMipsCode(Code line);
Code getMipsCode(Code line);


#endif
