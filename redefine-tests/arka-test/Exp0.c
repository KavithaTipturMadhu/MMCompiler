
int mul(int a, int b) { return a*b; }
int fmaX(int a, int b, int c) { return mul(a,b); }

void redefine_start() {
	int x = 10;
	int y = 11;
	int w = 10;
	int z =20;
  z = mul(x,y);
	w = z++;
	x = fmaX(y,w,z);
}
