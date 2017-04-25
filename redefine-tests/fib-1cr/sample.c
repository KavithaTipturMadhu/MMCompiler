int func(int x){
	if(x<1){
		return x;
	}
	return func(x-1);
}

void redefine_start(){
	func(10);
}
