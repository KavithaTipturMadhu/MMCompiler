
class myclass{
int a;

public:
int geta(){
return a;
}
};

void redefine_start(){
myclass b;
int x=b.geta();
}
