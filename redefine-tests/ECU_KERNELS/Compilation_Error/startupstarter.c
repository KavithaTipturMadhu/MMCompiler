#define SG_MODE_STARTER					1
#define GP_WARMUP_DELAY					7
#define SG_RAMP_DURATION				10
//#define SG_RAMP_MAX						0.6
//#define NH_STARTER_SHUTOFF				17000
//#define SG_MAX_RPM						15000
//#define SG_MAX							0.9
#define SG_MODE_IDLE					0
#define	SG_MODE_GENERATOR				2

//int redefine_in_a[6];
int redefine_in_i[3];
//signed int redefine_in_startup_time;
//int redefine_in_sg_bit0;
//int redefine_in_sg_bit1;

float redefine_in_f[3];
//float redefine_in_current_nh;
//float redefine_in_sg_cmd;
//float nh_fin_de_rampe;

int glob_sg_bit0, glob_sg_bit1;
float nh_fin_de_rampe;

float redefine_out_f[2];
//float sg_cmd;
//float nh_fin_de_rampe;
int redefine_out_i[2];
//int redefine_out_sg_bit0;
//int redefine_out_sg_bit1;

float SG_RAMP_MAX = 0.6, SG_MAX = 0.9;
int NH_STARTER_SHUTOFF = 17000;
int SG_MAX_RPM = 15000;

float startup_starter(int input_i[], float input_f[]) {//, float *sg_cmd, int *sg_bit0, int *sg_bit1) {
	float current_nh = redefine_in_f[0];
	float sg_cmd = redefine_in_f[1];

	int first = GP_WARMUP_DELAY * 50;
	int second = (GP_WARMUP_DELAY + SG_RAMP_DURATION) * 50;
	if (input_i[0] == 0) {
		/* Get the starter ready to spin */
		sg_cmd = 0;
		//asl_set_sg_mode(SG_MODE_STARTER);//, sg_cmd, sg_bit0, sg_bit1);

		glob_sg_bit0 = 1;
		glob_sg_bit1 = 0;
	}
	else if (input_i[0] < first) {
		/* Do nothing and wait for the glow plug to heat up */
	}
	else {
		//sg_cmd = foo(current_nh, sg_cmd);
		
		if (input_i[0] <= second) {
			sg_cmd = SG_RAMP_MAX * (float) (input_i[0] - (GP_WARMUP_DELAY * 50)) / (float) ((SG_RAMP_DURATION) * 50);
			//sg_cmd = SG_RAMP_MAX * div((input[1] - (GP_WARMUP_DELAY * 50)), ((SG_RAMP_DURATION) * 50));
			nh_fin_de_rampe = current_nh;
		} else if (current_nh < NH_STARTER_SHUTOFF) {
			//sg_cmd = div(5,1);
			if (current_nh < SG_MAX_RPM) {
				sg_cmd = SG_RAMP_MAX + (current_nh - nh_fin_de_rampe) / (SG_MAX_RPM - nh_fin_de_rampe) * (SG_MAX - SG_RAMP_MAX);
				if (sg_cmd < SG_RAMP_MAX) {
					sg_cmd = SG_RAMP_MAX;
				}
				if (sg_cmd > SG_MAX) {
					sg_cmd = SG_MAX;
				}
			}
		} else {
			//asl_set_sg_mode(SG_MODE_IDLE);//, sg_cmd, sg_bit0, sg_bit1);
            		sg_cmd = 0;
			glob_sg_bit0 = 0;
			glob_sg_bit1 = 0;
		}
	}
    return sg_cmd;
}

void redefine_start()
{
	glob_sg_bit0 = redefine_in_i[1];
	glob_sg_bit1 = redefine_in_i[2];
	nh_fin_de_rampe = redefine_in_f[2];

	redefine_out_f[0] = startup_starter(redefine_in_i, redefine_in_f);
	redefine_out_f[1] = nh_fin_de_rampe;

	redefine_out_i[0] = glob_sg_bit0;
	redefine_out_i[1] = glob_sg_bit1;
}

