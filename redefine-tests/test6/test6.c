int test(){
	typedef struct s{
	int a;
	char *b;
	}S;

	S s1,s2;
	s1.a = 12;
	s1.b = "preetam";
	s2.a = s1.a++;
	s2.b = "preetam";
	if(s1.a == s2.a)
		return s1.a;
	else
		return s2.a;
}
