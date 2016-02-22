#include <stdio.h>

int main()
{
	int a,b,c;
	a=1;
	b=1;
	c=1;
	while(1){
		if(c<100){
			if(b<20){
				b=a;
				c=c+1;
			}
			else{
				b=c;
				c=c+1;				
			}
		}else{
		   return 1;		
		}
	}
	return 1;
	
}
