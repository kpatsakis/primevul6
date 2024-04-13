decompileTRY(int n, SWF_ACTION *actions, int maxn)
{
#ifdef DEBUG
	struct _stack *StackSave=Stack;
#endif    
	OUT_BEGIN2(SWF_ACTIONTRY);
	INDENT
	println("try {");
	decompileActions(sact->numTryActs, sact->TryActs,gIndent+1);
	INDENT
	println("}");
#ifdef DEBUG
	if (Stack!=StackSave)
	{
		println("/* Stack problem in try{} code above */");
		Stack=StackSave;
	}
#endif
	if (sact->numCatchActs)    
	{
		struct SWF_ACTIONPUSHPARAM *rsave=NULL;
		INDENT
		if( ! sact->CatchInRegisterFlag)
			println("catch (%s) {",sact->CatchName);
		else
		{
			char *t=malloc(5); /* Rddd */
			sprintf(t,"R%d", sact->CatchRegister );
			rsave=regs[sact->CatchRegister];
			regs[sact->CatchRegister] = newVar(t);
			println("catch (%s) {",t);
		}
		decompileActions(sact->numCatchActs, sact->CatchActs,gIndent+1);
		INDENT
		println("}");
		if (rsave)
			regs[sact->CatchRegister]=rsave;
#ifdef DEBUG
		if (Stack!=StackSave)
		{
			println("/* Stack problem in catch{} code above */");
			Stack=StackSave;
		}
#endif
	} 
	if (sact->numFinallyActs)
	{
		INDENT
		println("finally () {");
		decompileActions(sact->numFinallyActs, sact->FinallyActs,gIndent+1);
		INDENT
		println("}");
#ifdef DEBUG
		if (Stack!=StackSave)
		{
			println("/* Stack problem in finally{} code above */");
			Stack=StackSave;
		}
#endif
	}
	return 0;
}