/* Tests CSE
 * 1 - replace b+c in e=(b+c)*f by temporary
 * 2 - replace i=(b+c+d)*a by a*a
 * 3 - should not replace in j = b+c+d+a 
*/
#include <stdio.h>

int main()
{
	int a,b,c,d,i,j;
	a = b+j;
	d= j + b;
	i=10;
	a = b+i;
	d = b+j;
	a = b + c;
	return 0;
}
