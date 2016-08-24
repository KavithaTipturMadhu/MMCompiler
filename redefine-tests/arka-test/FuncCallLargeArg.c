int LargeArg(int a, int b, int c, int d, int e) { return a+b+c+d+e; }

void redefine_start() {
	int a = 9;
	int b;
	b = LargeArg(a,2,3,4,5);
}
