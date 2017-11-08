#define SG_MODE_STARTER					1
#define GP_WARMUP_DELAY					7
#define SG_RAMP_DURATION				10
//#define SG_RAMP_MAX						0.6
//#define NH_STARTER_SHUTOFF				17000
//#define SG_MAX_RPM						15000
//#define SG_MAX							0.9
#define SG_MODE_IDLE					0
#define	SG_MODE_GENERATOR				2

int redefine_in_a[6];
int glob_sg_bit0, glob_sg_bit1;
float nh_fin_de_rampe;
//float redefine_in_current_nh;
//signed int redefine_in_startup_time;
//float redefine_in_sg_cmd;
//int redefine_in_sg_bit0;
//int redefine_in_sg_bit1;
//float nh_fin_de_rampe;

float redefine_out_b[2];
//float sg_cmd;
//float nh_fin_de_rampe;
int redefine_out_c[2];
//float redefine_out_sg_cmd;
//int redefine_out_sg_bit0;
//int redefine_out_sg_bit1;

float SG_RAMP_MAX = 0.6, SG_MAX = 0.9;
int NH_STARTER_SHUTOFF = 17000;
int SG_MAX_RPM = 15000;
/*
void asl_set_sg_mode(int mode) {//, float *sg_cmd_cpy, int *sg_bit0, int *sg_bit1) {
	
	if(mode == SG_MODE_IDLE) {
		glob_sg_cmd = 0;
		glob_sg_bit0 = 0;
		glob_sg_bit1 = 0;
	}
	if(mode == SG_MODE_STARTER) {
		glob_sg_bit0 = 1;
		glob_sg_bit1 = 0;
	}
	if(mode == SG_MODE_GENERATOR) {
		glob_sg_bit0 = 1;
		glob_sg_bit1 = 1;
	}
	else {}
}

float div(float num, float den) {
	int int_num = *((int*)&num);
	int int_den = *((int*)&den);
	int bias = 0x7F;
	int zero = 0;
	int one = 1;
	int two = 2;
	int sign_bit = 0; //(num >> 31) ^ (den >> 31); Sometimes doesn't work but works in host system
	int num_exponent = (int_num & 0x7F800000) >> 23;
	int den_exponent = (int_den & 0x7F800000) >> 23;
	int num_mantissa = (int_num & 0x007FFFFF) + 0x00800000;
	int den_mantissa = (int_den & 0x007FFFFF) + 0x00800000;
	int result_exponent = num_exponent - den_exponent + bias;
	int mantissa_quotient = 0x00000000;
	int increment_bit = 0x00800000;
	int result;
	
	if((int_num & 0x80000000) == one) {
		if((int_den & 0x80000000) == zero)
			sign_bit = one;
	} else {
		if((int_den & 0x80000000) == one)
			sign_bit = one;
	}

	if(num_mantissa < den_mantissa) {
		num_mantissa = num_mantissa * two;			//Problem is here, can't use immediate values
		result_exponent = result_exponent - one;
	}

	while(increment_bit > zero) {

		if(num_mantissa >= den_mantissa) {
			num_mantissa = num_mantissa - den_mantissa;
			mantissa_quotient = mantissa_quotient + increment_bit;
		} else if(num_mantissa == zero)
			break;
		
		num_mantissa = num_mantissa << one;
		increment_bit = increment_bit >> one;
	}

	result = (sign_bit << 31) + (result_exponent << 23) + (mantissa_quotient & 0x007FFFFF);

	return *((float*)&result);
}*/
/*
float foo(float current_nh, float sg_cmd)
{
	int second = (GP_WARMUP_DELAY + SG_RAMP_DURATION) * 50;
	if (redefine_in_a[1] <= second) {
		//float temp1 = startup_time - (GP_WARMUP_DELAY * 50);
		//float temp2 = (SG_RAMP_DURATION) * 50;
		//div(5, 2);
		sg_cmd = SG_RAMP_MAX * div((redefine_in_a[1] - (GP_WARMUP_DELAY * 50)), ((SG_RAMP_DURATION) * 50));
		nh_fin_de_rampe = current_nh;
	} else if (current_nh < NH_STARTER_SHUTOFF) {
		//sg_cmd = div(5,1);
		if (current_nh < SG_MAX_RPM) {
			sg_cmd = SG_RAMP_MAX + div((current_nh - nh_fin_de_rampe), (SG_MAX_RPM - nh_fin_de_rampe)) * (SG_MAX - SG_RAMP_MAX);
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
	return sg_cmd;
}
*/
float startup_starter(int input[]) {//, float *sg_cmd, int *sg_bit0, int *sg_bit1) {
	float current_nh = *((float*)&input[0]);
	float sg_cmd = *((float*)&input[2]);

	int first = GP_WARMUP_DELAY * 50;
	int second = (GP_WARMUP_DELAY + SG_RAMP_DURATION) * 50;
	//glob_sg_cmd = SG_RAMP_MAX * div((startup_time - (GP_WARMUP_DELAY * 50)), ((SG_RAMP_DURATION) * 50));
	if (input[1] == 0) {
		/* Get the starter ready to spin */
		sg_cmd = 0;
		//asl_set_sg_mode(SG_MODE_STARTER);//, sg_cmd, sg_bit0, sg_bit1);

		glob_sg_bit0 = 1;
		glob_sg_bit1 = 0;
	}
	else if (input[1] < first) {
		/* Do nothing and wait for the glow plug to heat up */
	}
	else {
		//sg_cmd = foo(current_nh, sg_cmd);
		
		if (input[1] <= second) {
			//float temp1 = startup_time - (GP_WARMUP_DELAY * 50);
			//float temp2 = (SG_RAMP_DURATION) * 50;
			//div(5, 2);
			sg_cmd = SG_RAMP_MAX * (float) (input[1] - (GP_WARMUP_DELAY * 50)) / (float) ((SG_RAMP_DURATION) * 50);
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
	glob_sg_bit0 = redefine_in_a[3];
	glob_sg_bit1 = redefine_in_a[4];
	nh_fin_de_rampe = *((float*)&redefine_in_a[5]);

	redefine_out_b[0] = startup_starter(redefine_in_a);
	redefine_out_b[1] = nh_fin_de_rampe;

	redefine_out_c[0] = glob_sg_bit0;
	redefine_out_c[1] = glob_sg_bit1;
}
