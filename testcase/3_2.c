int fact(int n)
{
	if (n==1)
		return n;
	else
		return (n * fact(n-1));
}

int main()
{
	int m, ret;
	m = read();
	if (m>1)
		ret = fact(m);
	else
		ret = 1;
	write(ret);
	return 0;

}
