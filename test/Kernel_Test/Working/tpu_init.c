//int redefine_in_parameters[7];
//int redefine_out_parameters[7];
int redefine_out_TPUMCR[2];
int redefine_in_placeholder;

struct TP3_TAG{
	int TPUMCR1;
	int TPUMCR2;
	int TPUMCR3;
};
int trp_init(struct TP3_TAG* tp3)
{

	int i;

	int psl;

	tp3->TPUMCR1= redefine_in_placeholder;

	tp3->TPUMCR2 = 0;
	tp3->TPUMCR3 = 1;

	i = 0;
	
	int psl_filler = 0;
	for (;psl_filler < 5; psl_filler++) {
		++i;
	}
	

	i = (i & 0x02);
	return i;
}

void redefine_start()
{
	struct TP3_TAG* tpu;
	//int temp = 54;
	//redefine_out_TPUMCR = trp_init(tpu);
	redefine_out_TPUMCR[1] = trp_init(tpu);
	redefine_out_TPUMCR[0] = tpu->TPUMCR1;
	//redefine_out_TPUMCR[1] = tpu->TPUMCR2;
	//redefine_out_TPUMCR[2] = tpu->TPUMCR3;

}


