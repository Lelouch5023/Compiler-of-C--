/*				tree file			*/
#include "head.h"
extern int yylineno;

pnode makenode(char* name, int number, ...)
{
    va_list arg;
    va_start(arg, number);
 
    pnode x = (pnode) calloc(1, sizeof(node));
    /* set childnum, name, type */
	x->childnum = number;
	x->type = 0;
	x->typesys = (Type) calloc(1, sizeof(struct Type_));
	x->typesys->kind = -1;			// kind<0 denotes nodes without attributes
	strcpy(x->name, name);
	x->line = yylineno;
	/* set children */    
    int i = 0;
    for (i = 0; i < number; i++)
    {
        x->child[i] = va_arg(arg, struct __node*);
		if (x->child[i]!=NULL)
		{
			x->child[i]->parent = x;
		}
	}
	if (number > 0 && x->child[0]!=NULL) x->line = x->child[0]->line;		// Referrence from Xiong Qiao
	/* Note: this makenode does not set the data 
	because x->type==0 for default, which means 
	carrying no data!*/
    va_end(arg);
    return x;
}



void treeTrav(pnode t)
{
	treePrint(t, 0);  
}

void treePrint(pnode t, int n)
{
  	if (t==NULL)	/*I wasted an hour on this!!!! I omitted it!!*/
		return ;
   	//print the node accoding to its type
  	int k;
	for (k = 0; k < n; k++)
		printf("    ");	/* on purpose */
   
  	printf("%s ", t->name);
  	if (t->type!=0)
		printf(":\"%s\"\n", t->data);
	else 
		printf("(%d)\n", t->line);
   
   	//print its children recursively
   	int N = t->childnum;
   	for (k = 0; k < N; k++)
   	{	
   	    treePrint(t->child[k], n+1);
   	}
}







/**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

void syntaxAnalysis(pnode t)
{
	
	dfvisit(t);
	printSymbolTable();
	
}





/***********   Error Info Stack *************/

pError newErrStack() 	/* initialize an error stack */
{
	pError pe = (pError) calloc(1, sizeof(Error));
	pe->errortype = -1;
	strcpy(pe->msg, ">>Error Report Complete\n");
	return pe;
}

void push(pError* stack, char* s, int line, int errortype)	/* push a error info into an Error Stack */
{
	// Form an error node in error info stack 
	pError e = (pError) calloc(1, sizeof(Error));
	strcpy(e->msg, s);
	e->line = line;
	e->errortype = errortype;
	
	// Insert this error node
	e->next = *stack;
	*stack = e;
}

void pop(pError stack)		/* pop out ALL error info and print out */
{
	pError tmp;
	while (stack!=NULL)
	{
		if (stack->errortype >= 0)
			printf("Error type %d at line %d: %s\n",
				stack->errortype, stack->line, stack->msg);
		else
			printf("%s", stack->msg);
		tmp = stack;
		stack = stack->next;
		free(tmp);
	}
}




/*
int main()
{
	pError stk = newErrStack();
	push(&stk, "no such thing", 1, 2);
	push(&stk, "conflicting type", 3, 6);
	push(&stk, "redefined identifier", 4, 3);
	push(&stk, "constant cannot be assigned", 17, 25);
	push(&stk, "I cannot understand you!", 9, 8);
	push(&stk, "you fuck off", 5, 22);
	pop(stk);
	return 0;
}
*/
