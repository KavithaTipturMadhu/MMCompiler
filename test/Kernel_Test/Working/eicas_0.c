int redefine_in_eicas_egt;
int redefine_out_eicas_egt;

int eicas_calculator(int eicasin)
{
	
	int eicasout;
	if(eicasin < 0)
		eicasout = 0;
	else if(eicasin > 0xffff)
		eicasout = 0xffff;
	else
		eicasout = eicasin;
	/*
	else(eicasin <= 0xffff)
		eicasout = eicasin;
	*/
	return eicasout;
} 

void redefine_start()
{
	redefine_out_eicas_egt = eicas_calculator(redefine_in_eicas_egt);
}

