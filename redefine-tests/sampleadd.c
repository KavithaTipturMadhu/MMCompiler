int first(){
	return 10;
}

int second(){
	return 20;
}
int sum(){
	return first()+second();
}

void redefine_start(){
	int c = sum();
}

