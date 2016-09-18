int diff(int x){
	return x--;
}

int sum(int x){
	if(x>10){
		return x-1;
	}
	return x;
}

int redefine_out_val;
void redefine_start(){
	redefine_out_val = sum(20);
}

