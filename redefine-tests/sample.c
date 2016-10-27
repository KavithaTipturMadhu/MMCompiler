void add(int *a){
	int* x=a;
}

void redefine_start(){
	int a=10, b;
	add(&b);	
	b=a*10;
}
