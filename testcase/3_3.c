int power(int a, int n)
{//@requires n>=0	
	int ret = 1;
	
	while (n>0)
	{
		if (n/2*2!=n)	//odd number
		{
			ret = ret * a;
		}
		a = a * a;
		n = n/2;
	}
	
	return ret;
}


int sub(int x, int y)
{
	return (x - y);
}


int main()
{
	int p = read();
	int q = read();
	write(power(p, q));
	write(sub(p, q));
	return 0;
}

