decompileSETVARIABLE(int n, SWF_ACTION *actions,int maxn,int islocalvar)
{
	struct SWF_ACTIONPUSHPARAM *val, *var;

	val = pop();
	var = pop();
	if (val->Type!=12)
	{
		INDENT
	}
#ifdef DEBUG
	printf("*SETVariable* varName %s (type=%d)  valName=%s (type=%d)\n",
	       getName(var),var->Type, getName(val),val->Type);
#endif
	if (val->Type!=12 && islocalvar)
	{
		puts("var ");
	}
	if (gIndent<0)	/* the ENUM workaround:  */
	{			/* in "for (xx in yy) { }" we need xx, but nothing else */
		puts(getName(var));
		return 0;
	}


	switch (val->Type)
	{
	case 10:	
		puts(getName(var));		// Variable (NEVER as string)
		printf(" = " );
		decompilePUSHPARAM(val,0);
		println(";");
		break;		
	case 11:				/* simply output variable and inc/dec op */
		puts(getName(val));
		println(";");
		break;
	case 12:				/* do nothing: inline increment/decrement (using side effect only) */
		val->Type = PUSH_VARIABLE;     		// but print next time  e.g. in y=++x;
		break;
	default:	
		puts(getName(var));
		printf(" = " );
		decompilePUSHPARAM(val,1);	// for certain types parameter 1 does not care
		println(";");
	}
	return 0;
}