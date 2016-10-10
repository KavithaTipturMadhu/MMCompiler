int sum(int x){
	return x+10;
}

int diff(int x){
	return sum(x)+sum(x);
//+sum(x-3);
}

void redefine_start(){
	diff(10);
}
