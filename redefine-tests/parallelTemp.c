int __builtin_createinst(int, int);
int __builtin_falloc(int);
void __builtin_fdelete(int, int);
void __builtin_fbind(int, int, int);
void __builtin_sync(int, int, int);
void __builtin_writecm(int, int, int);
void __builtin_writecmp(int, int, int);

void sum(int a, int b){
	int j;
	for(j=0; j < 10; j++){
		a++;
	}
	__builtin_writecm(b, a, 0);
}

void redefine_start(){
	int a=10, b=20, c, d;
	c = __builtin_createinst(a, 1);
	__builtin_fbind(a, c, 2);
	__builtin_sync(a, c, 60);
	__builtin_writecm(c, b, 60);
	__builtin_writecmp(c, b, 60);
	d = __builtin_falloc(b);
	__builtin_fdelete(d, 1);
}
