#define mymacro 10
int test(){
int a,b,c,g;
char e,f;
float x,y;

g=0x111;
e='w';
x=3.1415;
y=0.14122;

if((g << 1) > (g >>1)){
    if((int)e > x){
        return e;
    }else{
        if(x*y > x)
            return x;
        else
            return y;
    }
}

return a+mymacro;
}
