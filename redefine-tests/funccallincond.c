//External inputs and outputs
int sum(int a, int b)
{
	return (a+b);
}
void redefine_start()
{
	int i=0,j=1,k=2,l=0;

	if(j+k>0)
	{
		if(j>0)
			i=10;
		else
			i=20;

		if(k>10)
			i=30;
		else
			i=40;
	}
	else
		l=sum(j,k);

}
