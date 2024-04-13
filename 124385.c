decompileSETMEMBER(int n, SWF_ACTION *actions, int maxn)
{
	struct SWF_ACTIONPUSHPARAM *val, *var, *obj;
	val = pop();
	var = pop();
	obj = pop();

#ifdef DEBUG
	printf("*SETMember* varName %s (type=%d)  objName=%s (type=%d)\n",getName(var),var->Type, getName(obj),obj->Type);
#endif
	if (obj->Type == 12)				/* do nothing: inline inc/dec using side effect */
	{
		obj->Type = PUSH_VARIABLE;		/* ...but only once */
		return 0;
	}
	INDENT
	if (obj->Type == 11)				/* simply output variable and inc/dec op */
	{
		decompilePUSHPARAM(obj,0);
		println(";");
		return 0;
	}

	decompilePUSHPARAM(obj,0);
	if (var->Type == PUSH_INT || var->Type == PUSH_DOUBLE || var->Type == PUSH_VARIABLE
	    || var->Type == PUSH_REGISTER || var->Type == 12 )
	{
		puts("[");
	}
	else
	{
		puts(".");
		if (OpCode(actions, n-1, maxn) == SWFACTION_STOREREGISTER)
		{
			struct SWF_ACTIONSTOREREGISTER *sactv2 = (struct SWF_ACTIONSTOREREGISTER*)&actions[n-1];
			if (sactv2->Register==0)
				regs[0]=newVar3(getName(obj),".",getName(var));		// easter 07: some sugar for mtc et al.
		}
	}
	decompilePUSHPARAM(var,0);
	if (var->Type == PUSH_INT || var->Type == PUSH_DOUBLE || var->Type == PUSH_VARIABLE
		|| var->Type == PUSH_REGISTER || var->Type == 12 )
	{
		puts("]");
	}
	printf(" = " );


	if ( OpCode(actions, n-1, maxn) == SWFACTION_STOREREGISTER ) {
		struct SWF_ACTIONSTOREREGISTER *sr =
			(struct SWF_ACTIONSTOREREGISTER*)&actions[n-1];
		printf("R%d", sr->Register);
	}
	else if (val->Type != PUSH_VARIABLE) {
		/* later it will be a switch{} */
		decompilePUSHPARAM(val,1);
	}
	else {
		decompilePUSHPARAM(val,0);
	}
	println(";");
	return 0;
}