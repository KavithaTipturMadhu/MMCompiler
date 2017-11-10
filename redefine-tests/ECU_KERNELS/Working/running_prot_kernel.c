#define PROTECTION_OIL_PRESS	0x00000004
#define PROTECTION_FLAME_OUT	0x00000020
#define PROTECTION_EPU_TEMP		0x00000010
#define MODE_STOP							0x00000010
#define MAX_EPU_TEMP					116

float var = 0.7F;

float redefine_in_f[2];
int redefine_in_i[4];
int redefine_out_b[2];

int *running_protection(float input_f[], int input_i[]) {
	float oil_press = input_f[0];
	float dtisa = input_f[1];

	if (oil_press < var) {
		input_i[3] = MODE_STOP;
		input_i[2] = PROTECTION_OIL_PRESS;
	}

	if (input_i[0] < (350 + (2*((int) dtisa)))) {
		input_i[3] = MODE_STOP;
		input_i[2] = PROTECTION_FLAME_OUT;
	}

	if (input_i[1] > MAX_EPU_TEMP){
		input_i[3] = MODE_STOP;
		input_i[2] = PROTECTION_EPU_TEMP;
	}

	return input_i;
}

void redefine_start() {
	int *output = running_protection(redefine_in_f, redefine_in_i);
	redefine_out_b[0] = output[2];
	redefine_out_b[1] = output[3];
}
