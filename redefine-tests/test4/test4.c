int test(){
int a, b, c, d, e, f, g, t, maxVal;
a=1;
b=5;
c=2;
d=8096;
t=1;
maxVal = b;
while(t<maxVal){
 switch(t){
        case 1: e=t*a;break;
        case 2: e=t/3;break;
        default : e=e*t;
 }

 f=t+b;
 g=e+f;
 t++;
}
 if(g>20)
   return g;
 else return 20;
}
