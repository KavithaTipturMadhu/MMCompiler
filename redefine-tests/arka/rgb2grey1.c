//#include <stdio.h>
//#include <stdint.h>

int redefine_inout_test[48][64];

void redefine_start() {
  unsigned int rowCnt = 0;
  unsigned int colCnt = 0;
	unsigned int pixel  = 0;
	unsigned int r			= 0;
	unsigned int g			= 0;
	unsigned int b			= 0;

  for(rowCnt = 0; rowCnt<48; rowCnt++) {
    for(colCnt =0; colCnt<64; colCnt++) {
			
			pixel = redefine_inout_test[rowCnt][colCnt];
			r = (float)(pixel >> 16);
			g = (float)(pixel >> 8);
			b = (float)(pixel);
	
      redefine_inout_test[rowCnt][colCnt] = (int)((r * 0.3f)+
                                     (g * 0.6f)+
                                     (b * 0.1f));

    }
  }
}

//int main() {
//	//FILE fid = fopen("image.txt","r");
//	//if(fp == NULL) {
//	//	printf("Unable to Open File ");
//	//	exit(EXIT_FAILURE);
//	//}
//
//	unsigned int  x = 0x00342397;
//	unsigned char r = x;
//	unsigned char g = x >> 8; 
//	unsigned char b = x >> 16;
//	printf(" Size of unsigned int is %d\n ",sizeof(x));
//	printf(" Value of the variable pix = %x, r = %x\n, g = %x\n, b = %x\n ",x,r,g,b);
//	//fclose(fid);
//}
