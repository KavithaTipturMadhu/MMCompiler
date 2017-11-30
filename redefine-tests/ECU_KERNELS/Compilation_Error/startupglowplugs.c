#define NH_GLOWPLUGS_OFF 15000

//int redefine_in_a[4];
float redefine_in_f;
//float redefine_in_current_nh;

int redefine_in_i[3];
//signed int redefine_in_startup_time;
//int redefine_in_gp1_cmd;
//int redefine_in_gp2_cmd;

int redefine_out_b[2];
//int redefine_out_gp1_cmd;
//int redefine_out_gp2_cmd;


int *startup_glowplugs(float current_nh, int input_i[]) {// current_nh, signed int startup_time, int *gp1_cmd, int *gp2_cmd) {
	//float current_nh = input_f;
	if (input_i[0] == 0) {
		/* Turn glow plugs ON */
		input_i[1] = 1;
		input_i[2] = 1;
	}
	else if (current_nh < NH_GLOWPLUGS_OFF) {
		/* Leave them on */
		//input_i[1] = 2;
		//input_i[2] = 2;
	}
	else {
		input_i[1] = 0;
		input_i[2] = 0;
	}
    //global_gp2_cmd = gp2_cmd;
    return input_i;
}

void redefine_start()
{
	int *output = startup_glowplugs(redefine_in_f, redefine_in_i);
	redefine_out_b[0] = output[1];
	redefine_out_b[1] = output[2];
}
