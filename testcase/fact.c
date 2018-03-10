int main()
{
	int x = read();
	write(fact(x));

	return 0;
}

int fact(int n)
{
	int ret = 1;
	if (n<=1)
		return 1;
	while (n>1)
	{
		ret = ret * n;
		n = n - 1;
	}
	return ret;	
}
