struct a{
	int b;
	void c(){
		b=10;
	}
}
int main(){
	struct a x;
	x.c();
	return x.b;
}
