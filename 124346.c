decompileCALLMETHOD(int n, SWF_ACTION *actions, int maxn)
{
	struct SWF_ACTIONPUSHPARAM *meth, *obj, *nparam;
	meth=pop();
	obj=pop();
	nparam=pop();
	if (nparam->p.Integer>25)
	{
		INDENT
		println("// Problem getting method arguments (%d ignored) below:",
		        nparam->p.Integer);
		nparam->p.Integer=0;
	}
#ifdef DEBUG
	printf("*CALLMethod* objName=%s (type=%d) methName=%s (type=%d)\n",
		getName(obj), obj->Type, getName(meth), meth->Type);
#endif
	if (meth->Type == PUSH_UNDEF) 	/* just undefined, like in "super();" */
		push(newVar_N(getName(obj),"","","(", nparam->p.Integer,")"));
	else
	{
		if (meth->Type == PUSH_INT || meth->Type == PUSH_DOUBLE || meth->Type == PUSH_VARIABLE
		    || meth->Type == PUSH_REGISTER || meth->Type == 12 )
		{
			push(newVar_N(getName(obj),"[",getName(meth),"](", nparam->p.Integer,")"));
		}
		else
			push(newVar_N(getName(obj),".",getName(meth),"(", nparam->p.Integer,")"));
	}
	if (OpCode(actions, n+1, maxn) == SWFACTION_POP)
	{
		/* call method and throw away any result */
		INDENT
		puts(getName(pop()));
		println(";" );
		return 1;
	}
	return 0;
}