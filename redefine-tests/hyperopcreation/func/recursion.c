int diff(int x){
	return sum(x-1);		
}
int sum(int x){
	return diff(x-1);
}

void redefine_start(){
	sum(10);
}
