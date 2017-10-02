#define NH_GLOWPLUGS_OFF 15000

//float redefine_in_current_nh;
int redefine_in_a[4];
//float redefine_in_current_nh;
//signed int redefine_in_startup_time;
//int redefine_in_gp1_cmd;
//int redefine_in_gp2_cmd;

//int redefine_out_gp1_cmd;
//int redefine_out_gp2_cmd;
int redefine_out_b[2];

int *startup_glowplugs(int input[]) {// current_nh, signed int startup_time, int *gp1_cmd, int *gp2_cmd) {
	float current_nh = *((float*)&input[0]);
	if (input[1] == 0) {
		/* Turn glow plugs ON */
		input[2] = 1;
		input[3] = 1;
	}
	else if (current_nh < NH_GLOWPLUGS_OFF) {
		/* Leave them on */
	}
	else {
		input[2] = 0;
		input[3] = 0;
	}
    //global_gp2_cmd = gp2_cmd;
    return input;
}

void redefine_start()
{
	int *output = startup_glowplugs(redefine_in_a);
	redefine_out_b[0] = output[2];
	redefine_out_b[1] = output[3];
}
