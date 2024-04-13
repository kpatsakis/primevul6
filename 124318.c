decompileDEFINEFUNCTION(int n, SWF_ACTION *actions, int maxn, int is_type2)
{
	int i,j,k,m,r;
	struct SWF_ACTIONPUSHPARAM *myregs[ 256 ];
	struct _stack *StackSave;    
	struct SWF_ACTIONDEFINEFUNCTION2 *sactv2;
	struct strbufinfo origbuf;
	OUT_BEGIN2(SWF_ACTIONDEFINEFUNCTION);
	sactv2 = (struct SWF_ACTIONDEFINEFUNCTION2*)sact;

#ifdef DEBUG
	if(n+1 < maxn)
	{
		println("/* function followed by OP %x */", 
		        OpCode(actions, n+1, maxn));
	}
#endif
#if USE_LIB
	if (isStoreOp(n+1, actions,maxn) 
	    || (  *sact->FunctionName==0 && !is_type2 )
	    || (*sactv2->FunctionName==0 && is_type2  ))
	{
		origbuf=setTempString();	/* switch to a temporary string buffer */
	}
#endif
	puts("function ");
	if (is_type2)
	{
		for(j=1;j<sactv2->RegisterCount;j++) 
		{
			myregs[j]=regs[j];
			regs[j]=NULL;
		}
		r=1;
		if (sactv2->PreloadThisFlag)	regs[r++]=newVar("this");
		if (sactv2->PreloadArgumentsFlag)	regs[r++]=newVar("arguments");
		if (sactv2->PreloadSuperFlag)	regs[r++]=newVar("super");
		if (sactv2->PreloadRootFlag)	regs[r++]=newVar("root");
		if (sactv2->PreloadParentFlag)	regs[r++]=newVar("parent");
		if (sactv2->PreloadGlobalFlag)	regs[r++]=newVar("global");

		puts(sactv2->FunctionName);
		puts("(");

		for(i=0,m=0;i<sactv2->NumParams;i++) 
		{
			puts(sactv2->Params[i].ParamName);
			if ( sactv2->Params[i].Register)
			{
				 printf(" /*=R%d*/ ",sactv2->Params[i].Register);
				 regs[sactv2->Params[i].Register] = newVar(sactv2->Params[i].ParamName);
				 m++;					// do not count 'void' etc
			}
			if( sactv2->NumParams > i+1 ) puts(",");
		}
		println(") {" );
		if (r+m < sactv2->RegisterCount)
		{
			INDENT
			puts("  var ");
		}
		for(k=r;r<sactv2->RegisterCount;r++)
		{
			if (!regs[r])
			{
				char *t=malloc(5); /* Rddd */
				sprintf(t,"R%d", r );
				puts (t);
				if (k++ < sactv2->RegisterCount- m -1)
					puts(", ");
				else
					println(";" );
				regs[r]=newVar(t);
			}
		}
		StackSave=Stack;
		decompileActions(sactv2->numActions, sactv2->Actions,gIndent+1);
#ifdef DEBUG
		if (Stack!=StackSave)
		{
			println("/* Stack problem in function code above */");
		}
#endif
		Stack=StackSave;
		for(j=1;j<sactv2->RegisterCount;j++) 
			regs[j]=myregs[j];
	}
	else
	{
		puts(sact->FunctionName);
		puts("(");
		for(i=0;i<sact->NumParams;i++) {
			puts(sact->Params[i]);
			if( sact->NumParams > i+1 ) puts(",");
		}
		println(") {" );
		k=0;
		if (sact->Actions[0].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH)
		{
			struct SWF_ACTIONPUSH *sactPush=(struct SWF_ACTIONPUSH *)sact->Actions;
			for(i=0;i<sactPush->NumParam;i++)
			{
				if ((&(sactPush->Params[i]))->Type == PUSH_REGISTER) 
					k++;	/* REGISTER */
			}
			if (k)
			{
				INDENT
				puts("  var ");
				for(i=1;i<=k;i++)
				{
					char *t=malloc(5); /* Rddd */
					sprintf(t,"R%d", i );
					puts (t);
					if (i < k)
						puts(", ");
					else
						println(";" );
					regs[i]=newVar(t);
				}
			}
		}
		for(j=1;j<=k;j++) 
			myregs[j]=regs[j];
		StackSave=Stack;
		decompileActions(sact->numActions, sact->Actions,gIndent+1);
#ifdef DEBUG
		if (Stack!=StackSave)
		{
			println("/* Stack problem in function code above */");
		}
#endif
		Stack=StackSave;
		for(j=1;j<=k;j++) 
			regs[j]=myregs[j];
	}
	INDENT
	if (isStoreOp(n+1, actions,maxn) 
	    || ( *sact->FunctionName==0 && !is_type2 )
	    || (*sactv2->FunctionName==0 && is_type2 ))
	{
		puts("}");
#if USE_LIB
		push (newVar(dcgetstr()));	/* push func body for later assignment */
		setOrigString(origbuf);		/* switch back to orig buffer */
#else
		push (newVar("/* see function code above */"));	/* workaround only if LIB is not in use */
#endif
	}
	else
		println("}" );
	return 0;
}