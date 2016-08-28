
class obj{
	int a;
public:	
	void set(){
		a=10;
	}
};

class obj1:public obj{
	int a;
public:	
	void set1(){
		a=10;
	}
};

int main(){
	obj1 myobj;
	myobj.set1();
	myobj.set();
	return 1;
}
