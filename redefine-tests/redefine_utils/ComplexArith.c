//#define PI 3.1415926535
//
typedef unsigned int redefine_size_t;

/**
 * memcpy - Copy one area of memory to another
 * @dest: Where to copy to
 * @src: Where to copy from
 * @count: The size of the area.
 */
void *redefine_memcpy(void *dest, void *src, redefine_size_t count) {
	char *tmp = dest;
	char *s 	= src;
	while(count--)
		*tmp++ = *s++ ;
	return dest; 
}

// Basic Complex Data Type
//typedef struct Complex_t1{
//	float rel;
//	float img;
//} Complex_t;
//
//// Twiddle Factor Computation
//Complex_t W(int size, int k) {
//	float phi = (-2*PI/size)*k;
//	Complex_t w;
//	w.rel = __builtin_cosf(phi);
//	w.img = __builtin_sinf(phi);
//	return w; 
//}


///**
// * Packing is as shown below
// * -------------------------------------
// * |63						32|31								0|
// * |			img				|				rel				 |
// * -------------------------------------
// */
//long unsigned int pack(Complex_t z) {
//	long unsigned int zh = 0;
//	redefine_memcpy((void *)&zh, (void *)&z, sizeof(Complex_t));
//	return zh;
//}

//Complex_t unpack(long unsigned int zh) {
//	Complex_t z;
//	
//}
