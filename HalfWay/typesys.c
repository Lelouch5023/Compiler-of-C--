#include "head.h"
#include<stdio.h>
#ifndef HEAD_H
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
#endif

bool paramCMP(FieldList A, FieldList B)
{
	if ((A!=NULL && B==NULL) || (A==NULL && B!=NULL))
	{
		return false;
	}
	else if (A==NULL && B==NULL)
	{
		return true;
	}
	else	/* a!=NULL && b!=NULL */
	{
		return typeCMP(A->type, B->type) && fieldlistCMP(A->next, B->next);
	}
}

/*
 *			switch(a->kind)
			{
				case BASIC:
					return a->u.basic == b->u.basic;
				break;
				
				case ARRAY:
					return (a->u.array.size == b->u.array.size) && typeCMP(a->u.array.elem, a->u.array.elem);	
				break;
				
				case STRUCTURE:
					return fieldlistCMP(a->u.structure, b->u.structure);
				break;

				case FUNCTION:
					return (!strcmp(a->u.function.funcname, b->u.function.funcname)) 
						&& (!typeCMP(a->u.function.ret, b->u.function.ret)) 
						&& (!paramCMP(a->u.function.param, b->u.function.param));
				break;

				default:
					printf("Not Yet Initialized!\n");
					return false;
			}
	
 */



Type TypeAssign(Type* des, Type src)
{
	if (src==NULL)
	{
		*des = NULL;
		return *des;
	}
	if (des!=NULL && *des!=NULL)
		free(*des);
	*des = (Type) calloc(1, sizeof(struct Type_));
	(*des)->kind = src->kind;
	switch(src->kind)
	{
		BASIC:
		(*des)->u.basic = src->u.basic;
		break;

		ARRAY:
		(*des)->u.array.size = src->u.array.size; 
		TypeAssign(&(*des)->u.array.elem, src->u.array.elem);	
		break;
		
		STRUCTURE:
		fieldlistAssign(&(*des)->u.structure, src->u.structure);
		break;
		
		FUNCTION:
		//TODO
		break;

		default:
		printf("Not yet initialized type, still assigning\n");
		(*des) = src;
	}

	return *des;
}


bool fieldlistCMP(FieldList A, FieldList B)
{
	if ((A!=NULL && B==NULL) || (A==NULL && B!=NULL))
	{
		return false;
	}
	else if (A==NULL && B==NULL)
	{
		return true;
	}
	else	/* a!=NULL && b!=NULL */
	{
		return strcmp(A->name, B->name) && typeCMP(A->type, B->type) && fieldlistCMP(A->next, B->next);
	}
}

void fieldlistAssign(FieldList* des, FieldList src)
{
	if (des!=NULL && *des!=NULL)
		free(*des);
	
	if (src==NULL)
	{
		*des = NULL;
	}
	else
	{
		(*des) = (FieldList) calloc(1, sizeof(struct FieldList_));
		(*des)->name = (char*) calloc(DATALEN, sizeof(char));	
		strcpy((*des)->name, src->name);
		TypeAssign(&(*des)->type, src->type);   
		fieldlistAssign(&(*des)->next, src->next);
	}

}




bool typeCMP(Type a, Type b)
{
	if ((a!=NULL && b==NULL) || (a==NULL && b!=NULL) )
	{
		return false;
	}
	else if (a==NULL && b==NULL)
	{
		return true;
	}
	else	/* a!=NULL && b!=NULL */
	{
		if (a->kind!=b->kind)
		{
			return false;
		}
		else
		{
			switch(a->kind)
			{
				case BASIC:
					return a->u.basic == b->u.basic;
				break;
				
				case ARRAY:
					return (a->u.array.size == b->u.array.size) && typeCMP(a->u.array.elem, a->u.array.elem);	
				break;
				
				case STRUCTURE:
					return fieldlistCMP(a->u.structure, b->u.structure);
				break;

				case FUNCTION:
					return (!strcmp(a->u.function.funcname, b->u.function.funcname)) 
						&& (!typeCMP(a->u.function.ret, b->u.function.ret)) 
						&& (!paramCMP(a->u.function.param, b->u.function.param));
				break;

				default:
					printf("Not Yet Initialized!\n");
					return false;
			}
		}

	}
	
}

/*
struct A
{
	int x[10];
	int a;
};
struct A
{
	int x[10];
	float a;
}



int main()
{
	Type a = (Type) calloc(1, sizeof(struct Type_));
	Type b = (Type) calloc(1, sizeof(struct Type_));
	
	a->kind = STRUCTURE;
	b->kind = STRUCTURE;
	a->u.structure = (FieldList)calloc(1, sizeof(struct FieldList_));
	b->u.structure = (FieldList)calloc(1, sizeof(struct FieldList_));
		a->u.structure->name = "A";	
		b->u.structure->name = "A";
		a->u.structure->type 	
	return 0;
}
*/
