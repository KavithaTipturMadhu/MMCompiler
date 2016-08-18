//#include <stdio.h>

// Swap program using pointers
int a;
int b;
int c;

void swap(int a, int b, int *c) {
	*c = a+b;
}

//int echo(int x) { return x; }

void redefine_start() {
	a = 10;
	b = 19;
	swap(a,b,&c);
}

//int main() {
//	int a = 89;
//	int b = 11;
//	int c = 2;
//	swap(a,b,&c);
//	printf("The value of c is %d\n",c);
//}
