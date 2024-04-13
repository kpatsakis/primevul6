decompileINCR_DECR(int n, SWF_ACTION *actions, int maxn, int is_incr)
{
	int is_postop;
	struct SWF_ACTIONPUSHPARAM *var=pop();
	char *dblop=is_incr ? "++":"--";

	if((OpCode(actions, n, maxn) == SWFACTION_PUSHDUP
	     || OpCode(actions, n+1, maxn) == SWFACTION_PUSHDUP 
	     || OpCode(actions, n+1, maxn) == SWFACTION_SETVARIABLE)
	     || ( OpCode(actions, n-1, maxn) == SWFACTION_GETVARIABLE
	          &&  OpCode(actions, n+1, maxn) == SWFACTION_STOREREGISTER
	          &&  OpCode(actions, n+1, maxn) == SWFACTION_SETVARIABLE))
	{
		is_postop=(OpCode(actions, n-1, maxn) == SWFACTION_PUSHDUP)?1:0;
		if (is_postop)
			var = newVar2(getString(var),dblop);
		else
			var = newVar2(dblop,getString(var));
		if (OpCode(actions, n+1, maxn) == SWFACTION_SETVARIABLE)
		{
			var->Type=11;	/* later trigger printing variable inc/dec */	
		}
		else
		{
			var->Type=12;	/* later be quiet, see decompileSETVARIABLE() */
			if (is_postop)
			{
				pop();
				push(var);	/* will duplicate stacktop */
			}
		}
		push(var);
	}
	else
	{
		if((OpCode(actions, n-1, maxn) == SWFACTION_GETMEMBER &&
		    OpCode(actions, n+1, maxn) == SWFACTION_STOREREGISTER &&
		    OpCode(actions, n+2, maxn) == SWFACTION_SETMEMBER ) ||
		   (OpCode(actions, n-1, maxn) == SWFACTION_STOREREGISTER &&
	    	    OpCode(actions, n+1, maxn) == SWFACTION_SETMEMBER &&
		    OpCode(actions, n+2, maxn) == SWFACTION_PUSH ) ||
		   (OpCode(actions, n-1, maxn) == SWFACTION_PUSH &&
		    OpCode(actions, n+1, maxn) == SWFACTION_SETMEMBER)  ||
		   (OpCode(actions, n-3, maxn) == SWFACTION_GETMEMBER &&
		    OpCode(actions, n-2, maxn) == SWFACTION_PUSH &&
		    OpCode(actions, n-1, maxn) == SWFACTION_GETMEMBER &&
		    OpCode(actions, n+1, maxn) == SWFACTION_SETMEMBER &&
	    	((struct SWF_ACTIONPUSH *)&actions[n-2].SWF_ACTIONRECORD)->NumParam >= 4 
	    		/* 4: a pair of get/set - FIXME: add more analysis about stack here */))
		{		// incr/decr object variables with side effects
			is_postop= (OpCode(actions, n+1, maxn) == SWFACTION_SETMEMBER)?1:0;
			if (is_postop)
				var = newVar2(getString(var),dblop);
			else
				var = newVar2(dblop,getString(var));
			if (is_postop && OpCode(actions, n-1, maxn) == SWFACTION_PUSH) 
				pop();
			if(OpCode(actions, n+1, maxn) == SWFACTION_GETMEMBER) 
				pop();
			
			pop();
			pop();
			var->Type=12;	// to be quiet later in ...SETMEMBER()
			regs[0]=var;	// FIXME: r0 perhaps a ming special
			push(var);
			push(var);
			push(var);
		
			if (is_postop && OpCode(actions, n-1, maxn) == SWFACTION_PUSH ) 
				push(var);
			if (is_postop && OpCode(actions, n-1, maxn) == SWFACTION_GETMEMBER ) 
				push(var);
		}
		else
		{
			if(OpCode(actions, n-1, maxn) == SWFACTION_PUSH &&
			   OpCode(actions, n+1, maxn) == SWFACTION_STOREREGISTER &&
			   regs[actions[n+1].SWF_ACTIONSTOREREGISTER.Register]->Type == PUSH_VARIABLE)
			{
				var = newVar2(dblop,getString(var));
				if ((OpCode(actions, n+2, maxn) == SWFACTION_POP 
				     && actions[n-1].SWF_ACTIONPUSH.NumParam==1) 
				    || OpCode(actions, n+3, maxn) == SWFACTION_POP)
				{
					var->Type=11;	// later print inc/dec
				}
				else
				{
					var->Type=12;	// later be quiet in ..STOREREGISTER()
					if (actions[n-1].SWF_ACTIONPUSH.NumParam>1) 
					{
						pop();
						push(var);
					}
				}
				push(var);
			}
			else		// fallback to old incr/decr code
			{		// FIXME: this is bad designed for handling side effect code
				INDENT	//        like post-incrementing a function argument etc.
				decompilePUSHPARAM(var,0);
				puts(dblop);
				println(";");
				push(var);
			}
		}
	}
	return 0;
}