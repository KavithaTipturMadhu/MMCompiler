// Function calls inside conditions (if-else and case constructs)
//#define TRUE 	1
//#define FALSE 0

int mul(int a, int b) { return a*b; }
//int add(int a, int b) { return a+b; }
//int sub(int a, int b) { return a-b; }

//int isMul(char x) {
//	if(x == '*') {
//		return TRUE;
//	}
//	else {
//		return FALSE;
//	}
//}

void redefine_start() {
	char op = '+';
	int c = 0;
	int a = 1;
	int b = 2;

	if(op == '*') {
		c = mul(a,b);
		//c = a*b;
	}
	else {
		//c = add(a,b);
		c = a+b;
	}
}
