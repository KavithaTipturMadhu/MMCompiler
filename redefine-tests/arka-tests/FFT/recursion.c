// Recursive Version of FFT, using the Simplest Radix-2 (DIT/DIF) FFT
#define PI 3.1415926535
#define SAMPLES 4
#define LOG2SAMPLES 2

typedef struct Complex_t {
	float rel;
	float img;
} Complex;

Complex redefine_in[SAMPLES];
Complex redefine_out[SAMPLES];

Complex TF(int n) {
	Complex W;
	int N = SAMPLES;
	W.rel = __builtin_cosf(2*PI*n/N);
	W.img = __builtin_sinf(2*PI*n/N);
	return W;
}

Complex Complex_add(Complex a, Complex b) {
	Complex returnData;
	returnData.rel  = a.rel + b.rel;
	returnData.img  = a.img + b.img;
	return returnData;
}

Complex Complex_sub(Complex a, Complex b) {
	Complex returnData;
	returnData.rel = a.rel - b.rel;
	returnData.img = a.img - b.img;
	return returnData;
}

Complex Complex_mul(Complex a, Complex b) {
	Complex returnData;
	returnData.rel = (a.rel*b.rel) - (a.img*b.img);
	returnData.img = (a.rel*b.img) + (a.img*b.rel);
	return returnData;
}

// (Recursive Version) Some Errors With This Code
void FFT(Complex* in, Complex* out, int N) {
	int i;
	Complex o[SAMPLES/2], O[SAMPLES/2];
	Complex e[SAMPLES/2], E[SAMPLES/2];

	// Odd/Even Decomposition
	for(i=0; i<N/2; i++) {
		e[i] = in[2*i];
		o[i] = in[2*i+1];
	}

	// Radix 2 FFT
	if(N == 2) {
		*out 		 = Complex_add(*in, *(in+1));
		*(out+1) = Complex_sub(*in, *(in+1));
	}
	else if (N > 2) {
		// Divide
		FFT(o,O,N/2);
		FFT(e,E,N/2);

		// Merge
		for(i=0; i<N; i++) {
			out[i] = Complex_add(E[i],Complex_mul(O[i],TF(i)));
		}
	}
}

/**
 * (Iterative N pt) FFT, In Place Computation
 * +1 for forward Computation
 * -1 for backward Computation
 */
//void FFT_iterative(Complex* x, int N, int log2N) {
//	
//}

void redefine_start() {
		FFT(redefine_in, redefine_out, 4);
}
