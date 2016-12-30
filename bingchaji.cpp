/*************************************************************************
    > File Name: bingchaji.cpp
    > Author: Hu Jian
    > Mail: hujian95@qq.com 
    > Created Time: 日 12/18 22:55:26 2016
 ************************************************************************/

#include <iostream>
#include <cstdio>

using namespace std;

int ret = 0;

//#define Num 50000
//int pre[Num];
//int deg[Num];	//
int *pre;
int *deg;

enum {
	A=0, B=1, C=2
};

void init(int N)
{
	int i;
	pre = new int[N];
	deg = new int[N];
	for (i = 0; i < N; i++)
	{
		pre[i] = i;
		deg[i] = -1;
	}

}


int find(int x)
{
	int r = x;
	int s, t;
	while (r!=pre[r])
	{
		if (deg[pre[r]]!=-1)
		{
			deg[r] = deg[pre[r]];
			deg[pre[r]] = deg[r];
		}
		if (deg[r]!=-1)
		{
			deg[pre[r]] = deg[r];
			deg[r] = deg[pre[r]];
		}
		r = pre[r];
	}
	
	s = x;
	while (s!=r)
	{
		t = pre[s];
		pre[s] = r;
		s = pre[t];
	}
	
	return r;
}
	
int join(int x, int y)
{
	int fx = find(x);
	int fy = find(y);

	if (fx < fy)
		pre[fy] = fx;
	else
		pre[fx] = fy;
	
	return fy;	//optional
}

int eat(int x, int y)	// x eats y
{
	int fx = find(x);
	int fy = find(y);
	
	return 0;
}


int main(int argc, char* argv[])
{
	int N, K;
	cin>>N>>K;
	init(N);
	
	int op, x, y;
	N--;	//for 012, 345, 678 group
	for (int i = 0, int j = 0; i < N; i++)
	{
		cin>>op>>x>>y;
		x--, y--;	//for 012, 345, 678 group
		if (i==0)	deg[x] = A;
		if (x > N || y > N)
		{
			ret ++;
			continue;
		}

		if (op==1)
		{
			int fx = find(x);
			int fy = find(y);
			if (deg[fx]!=-1 && deg[fy]!=-1)
			{
				if (deg[fx]==deg[fy])
				{
					continue;											
				}
				else
				{
					ret ++;
					continue;
				}
			}
			else if (deg[fx]==-1 && deg[fy]!=-1)
			{
				join(x, y);
				deg[x] = deg[y];
			}
			else if (deg[fx]!=-1 && deg[fy]==-1)
			{
				join(y, x);
				deg[y] = deg[x];
			}
			else	//deg[fx]==-1 && deg[fy]==-1
			{
				join(x, y);		
			}	

		}
		else //op==2
		{
		
			int fx = find(x);
			int fy = find(y);
			if (deg[fx]!=-1 && deg[fy]!=-1)
			{
				int N1, N2;	//group number
				int off1, off2;	//offsets
				N1 = deg[fx]/3;	//N1 = deg[x]/3;
				N2 = deg[fy]/3;
				off1 = deg[fx]%3;
				off2 = deg[fy]%3;
				if (N1==N2)
				{
					if (off1!=off2)
					{
						ret ++;
						continue;
					}
				}
				else
				{
					for (int k = 0; k < 3; k++)
					{
						join(N1*3+(off1+k)%3, N2*3+(off2+k)%3);	//skills get!
					}
				}
				
				
				
			}
			else if (deg[fx]==-1 && deg[fy]!=-1)
			{

			}
			else if (deg[fx]!=-1 && deg[fy]==-1)
			{

			}
			else	//deg[fx]==-1 && deg[fy]==-1
			{
				deg[fx] = i*3;	// -1 0,1,2 6,7,8 9,10,11
				deg[fy] = deg[fx]+1;
				
			}


			j++;
		}
		
		
	}

	cout<<ret;

    return 0;
}

