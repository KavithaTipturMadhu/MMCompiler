/**
 * Signed/Unsigned Integer tests with Intermediate vales of varying sizes
 * case 1 : x < 11 bits (including boundary cases like 0x00000000 - 0x000003ff)
 * case 2 : x = 11 bits (including boundary cases like 0x00000400 - 0x000007ff)
 * case 3 : 12 <= x < 32 bits (including boundary  like 0x00000800 - 0x7fffffff)
 * case 4 : x = 32 bits (including boundary cases like 0x80000000 - 0xffffffff)
 * case 5 : x > 32 bits (0x100000000 - unbounded)
 */
void redefine_start() {
	unsigned int a = 2048;
	//int b = 55;
	//int c = a+b;
}
