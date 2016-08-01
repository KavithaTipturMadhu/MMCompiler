int diff(int a, int b){
	return a-b;
}

int sum(int a, int b){
	return diff(a+b, a-b);
}

void redefine_start(){
	int a=10, b=20,c;
	c=sum(a,b);
	return;
}
