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
	
/*
 *		>>>>>>Type System Interface
 *
 */

struct Type_;
struct FieldList_;
enum {BASIC, ARRAY, STRUCTURE, FUNCTION};
enum {INT_TYPE, FLOAT_TYPE};

typedef struct Type_
{
	int kind;	/* BASIC, ARRAY, STRUCTURE, FUNCTION (minus value means disabled) */
	union {
		int basic;	/* INT_TYPE, FLOAT_TYPE */
		struct { struct Type_* elem; int size; } array;
		struct FieldList_* structure;
		struct { struct Type_* ret; struct FieldList_* param; char funcname[DATALEN];} function;//Extension
	} u;
} *Type;

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

/*
 *
 *			>>> Error Info Stack System
 *
 */

typedef struct __Error{
	char msg[128];
	int line;
	int errortype;
	struct __Error* next;
}Error, *pError;

void push(pError* stack, char* s, int line, int errortype);
void pop(pError stack);
pError newErrStack();


#endif
