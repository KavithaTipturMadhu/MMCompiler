int __builtin_foo(int, int);

int sum(int a){
	int j;
	for(j=0; j < 10; j++){
		a++;
	}
	return a;
}

void redefine_start(){
	int a=10, b=20, c;
//	c = sum(a);
	c = __builtin_foo(a, b);
}

