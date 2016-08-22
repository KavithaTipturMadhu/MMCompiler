#define PI 3.1415926535f

typedef struct Complex_t{
  float rel;    
  float img;
 } Complex;


//Complex getTwiddle(char i, char size){
//  char phi = (-2*PI/size)*i;
//  Complex w;
//  w.rel = __builtin_cosf(phi);
//  w.img = __builtin_sinf(phi);
//  return w;
//}

Complex Complex_add(Complex a, Complex b){
  Complex returnData;
  returnData.rel  = a.rel + b.rel;
  returnData.img  = a.img + b.img;
  return returnData;
}

Complex redefine_in_a;
Complex redefine_in_b;
Complex redefine_out_sum;

void redefine_start() {
		redefine_out_sum = Complex_add(redefine_in_a,redefine_in_b);
}

//Complex Complex_sub(Complex a, Complex b){
//  Complex returnData;
//  returnData.rel = a.rel - b.rel;
//  returnData.img = a.img - b.img;
//  return returnData;
//}
//
//Complex Complex_mul(Complex a, Complex b){
//  Complex returnData; 
//  returnData.rel = (a.rel*b.rel) - (a.img*b.img); 
//  returnData.img = (a.rel*b.img) + (a.img*b.rel);
//  return returnData; 
//}
//
//Complex redefine_in[2];
//Complex redefine_out[2];
//
//void fft2(Complex *x0, Complex *x1){
//  Complex tmp0 = *x0;  
//  Complex tmp1 = *x1;
//  *x0 = Complex_add(tmp0, tmp1);
//  *x1 = Complex_sub(tmp0, tmp1);
//}
//
//void redefine_start() {
//	fft2(redefine_in, redefine_out);
//}
