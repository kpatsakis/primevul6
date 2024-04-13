decompileRETURN(int n, SWF_ACTION *actions, int maxn)
{
	struct SWF_ACTIONPUSHPARAM *var=pop();
	INDENT
	printf("return ");
	if (var->Type== PUSH_REGISTER && var->p.RegisterNumber==0)	/* REGISTER 0 used as helper variable */
		puts(getName(regs[0]));
	else
		decompilePUSHPARAM(var,1);                                                                                             
	println(";");
	return 0;
}