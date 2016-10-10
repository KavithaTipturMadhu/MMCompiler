int a, b, c;

int sum(int* c){
	return *c=a+b;
}

void redefine_start(){
	sum(&c);	
}

