int diff(int a, int b){
	return a-b;
}

int sum(int a, int b){
	int c= diff(a+b, a-b);
	return c;
}

void redefine_start(){
	int a=10, b=20, c, d;
	c=sum(a,b);
	return;
}
