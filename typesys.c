#include "head.h"

#ifndef HEAD_H
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
		struct { char structname[DATALEN]; struct FieldList_* structfield } object;	//Extention : an object of struct/class
	} u;
} *Type;

typedef struct FieldList_	
{
	char* name;
	struct Type_* type
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
		return typeCMP(A->type, B->type)==true && fieldlistCMP(A->next, B->next)==true;
	}
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
		return strcmp(A->name, B->name)==0 && typeCMP(A->type, B->type)==true && fieldlistCMP(A->next, B->next)==true;
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
					return (a->u.array.size == b->u.array.size) 
					&& typeCMP(a->u.array.elem, a->u.array.elem)==true;	
				break;
				
				case STRUCTURE:
					return fieldlistCMP(a->u.structure, b->u.structure)==true;
				break;

				case FUNCTION:
					return (strcmp(a->u.function.funcname, b->u.function.funcname)==0) 
						&& (typeCMP(a->u.function.ret, b->u.function.ret)==true) 
						&& (paramCMP(a->u.function.param, b->u.function.param)==true);
				break;

				case OBJECT:
					/*
					*	Anonymous declaration: compare their fieldlist
					*	Real-name declaration: only compare their struct name
					*/
					if (strcmp(a->u.object.structname, b->u.object.structname)==0)
					{
						if(strcmp(a->u.object.structname, "")==0)
							return fieldlistCMP(a->u.object.structfield, b->u.object.structfield)==true;
						return true;
					}
					else
					{
						return false;
					}
				break;

				default:
					//printf("Not Yet Initialized!\n");
					return false;
			}
		}

	}
	
}


Type TypeAssign(Type* des, Type src)
{
	
	if (src==NULL)
	{
		*des = NULL;
		return *des;
	}
	/*
	if (des!=NULL && *des!=NULL)
		free(*des);
	*/
	*des = (Type) calloc(1, sizeof(struct Type_));
	(*des)->kind = src->kind;		// kind assignment
	
	switch(src->kind)		// u assignment
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
		TypeAssign(& (*des)->u.function.ret, src->u.function.ret);
		fieldlistAssign(& (*des)->u.function.param, src->u.function.param);
		strcpy((*des)->u.function.funcname, src->u.function.funcname);  
		break;

		OBJECT:
		strcpy((*des)->u.object.structname, src->u.object.structname);
		//fieldlistAssign((*des)->u.object.structfield, src->u.object.structfield);
		(*des)->u.object.structfield = src->u.object.structfield;	/* Caution! Here is just address assignment! */
		break;

		default:
		//printf("Not yet initialized type, still assigning\n");
		(*des) = src;
	}

	return *des;
}


void fieldlistAssign(FieldList* des, FieldList src)
{
	/*
	if (des!=NULL && *des!=NULL)
		free(*des);
	*/
	if (src==NULL)
	{
		*des = NULL;
	}
	else
	{
		(*des) = (FieldList) calloc(1, sizeof(struct FieldList_));
		(*des)->name = (char*) calloc(DATALEN, sizeof(char));	
		strcpy((*des)->name, src->name);			//name
		TypeAssign(& (*des)->type, src->type);   	//type
		fieldlistAssign(& (*des)->next, src->next);	//next
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
