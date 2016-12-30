struct A
{
	int p;
	int q;
	float ls, rm;
	int array[10];
};


struct A f(int u, float v[3])
{
	return 0;
}

int r[3][3];
/*
 *
struct A f(int y[10][20])
{	//是VarList里面的Segmentation Fault
	struct A fa;
	return fa;
}
*/


int main()
{
	int a, b[10];
	float c;
	struct A x;
	struct A m[10][20];
	a = a + c;
	return 0;
}
