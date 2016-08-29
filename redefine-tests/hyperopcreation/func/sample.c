int a, b, c;
void sum(int a, int b, int *c){
	*c = a+b;
}


void redefine_start(){
	sum(a, b, &c);
}
