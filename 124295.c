decompileIF(int n, SWF_ACTION *actions, int maxn)
{
	int offSave;
	int j,i=0;
	struct strbufinfo origbuf;
	OUT_BEGIN2(SWF_ACTIONIF);

        if (sact->numActions < 1) {
            return 0;
        }

	/*
	* IF is used in various way to implement different types
	* of loops. We try to detect these different types of loops
	* here.
	*/

#ifdef STATEMENT_CLASS
	if((OpCode(actions, n-1, maxn) == SWFACTION_LOGICALNOT) &&
	   (OpCode(actions, n-2, maxn) == SWFACTION_LOGICALNOT) &&
	   (OpCode(actions, n-3, maxn) == SWFACTION_GETVARIABLE) &&
	   (OpCode(actions, n-4, maxn) == SWFACTION_PUSH) ) 
	{
	    /* It's really a class definition */
		INDENT
		puts("class ");
		decompilePUSHPARAM(newVar(getName(pop())),0);
		println(" {" );
		decompileActions(sact->numActions, sact->Actions,gIndent+1);
		INDENT
		println("}");
		return 0;
	}

	if( 
	   (OpCode(actions, n-1, maxn) == SWFACTION_LOGICALNOT) &&
	   (OpCode(actions, n-2, maxn) == SWFACTION_LOGICALNOT) &&
	   (OpCode(actions, n-3, maxn) == SWFACTION_GETMEMBER) &&
	   (OpCode(actions, n-4, maxn) == SWFACTION_PUSH) ) 
	{
	    /* It's really a class definition */
		INDENT
		println(" {");
		decompileActions(sact->numActions, sact->Actions,gIndent+1);
		INDENT
		println("}");
		return 0;
	}
#endif
	/*
	 * do {} while() loops have a JUMP at the end of the if clause
	 * that points to a JUMP above the IF statement.
	 */
	if(n && isLogicalOp(n-1, actions, maxn) &&
	   (sact->Actions[sact->numActions-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_JUMP) &&
	   ( (sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.Offset +
	      sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset) < actions[n].SWF_ACTIONRECORD.Offset) &&
	      isLogicalOp(sact->numActions-2, sact->Actions, maxn) ) 
	{
		INDENT
		println("do {");
		offSave=offseoloop;
		offseoloop=actions[n].SWF_ACTIONRECORD.Offset+5;
		decompileActions(sact->numActions-1, sact->Actions,gIndent+1);
		offseoloop=offSave;
		INDENT
		puts("while( ");
		puts(getName(pop()));
		puts(");");
		return 0;
	}

	/* ak,2006
	 * lots of "do {} while()" have simply a CONDITIONED JUMP back at the end of the loop
	 */
	if( actions[n].SWF_ACTIONJUMP.BranchOffset < 0 ) 
	{
		INDENT
		println("do {                  /* 2nd type */ ");
		offSave=offseoloop;
		offseoloop=actions[n  ].SWF_ACTIONRECORD.Offset+5;
		decompileActions(sact->numActions, sact->Actions,gIndent+1);
		offseoloop=offSave;
		INDENT
		puts("} while( ");
		puts(getName(pop()));
		println(");");
		return 0;
	}

	j=0;
	while (OpCode(actions, n-j, maxn) != SWFACTION_ENUMERATE  && 
	        OpCode(actions, n-j, maxn) != SWFACTION_ENUMERATE2 && j<n && j<5) 
	{
		j++;		// check for a pending ENUMERATE
	}
	
	if ((OpCode(actions, n-j, maxn) == SWFACTION_ENUMERATE ||
	     OpCode(actions, n-j, maxn) == SWFACTION_ENUMERATE2 ) && 
	      OpCode(actions, n-j+1, maxn) == SWFACTION_STOREREGISTER )
	{
		struct SWF_ACTIONPUSHPARAM *var;
		int x;
		var = pop();
		INDENT
		puts("for ( ");
		// check for an usual special case w register Rx
		if (sact->Actions[1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER)
		{
			struct SWF_ACTIONSTOREREGISTER *sactv2 = (struct SWF_ACTIONSTOREREGISTER*)&sact->Actions[1];
			puts("var ");
			puts(getName(regs[sactv2->Register]));
			x=3;
		}
		else
		{
			decompileActions( 2 , sact->Actions,-1);   /* -1 == the ENUM workaround */
			x=2;
		}
		puts(" in ");
		puts(getName(var));
		println(" ) {");
		if(n+1 >= maxn)
		{
			SWF_warn("Warning: %s:%i: something is wrong here\n", __FILE__, __LINE__);
		}
		else 
		{
			offSave=offseoloop;
			offseoloop=actions[n+1].SWF_ACTIONRECORD.Offset;
			decompileActions(sact->numActions-1-x, &sact->Actions[x],gIndent+1);
			offseoloop=offSave;
		}
		INDENT
		println("}");
		return 0;
	}

	/*
	 * while() loops have a JUMP at the end of the if clause that jumps backwards
	 * But also "continue" statements could jump backwards.
	 */
	
	if( isLogicalOp(n-1, actions, maxn) &&
	    ( (sact->Actions[sact->numActions-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_JUMP) &&
	       sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset < 0) ) 
	{
		if(0)	    dumpRegs();
		INDENT
		/* if on a level >0 we can check for any outer loop 
		   To do: get the level on a better way than using gIndent */
		if (gIndent	
		    && OpCode(actions, maxn-1, maxn) == SWFACTION_JUMP
	  	    && actions[maxn-1].SWF_ACTIONJUMP.Offset+actions[maxn].SWF_ACTIONJUMP.BranchOffset==
	               sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.Offset+sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset)
		{      
		   /* this jump leads from a block to start of a loop on outer block:
		       it is an 'if' later followed by last action 'continue' */
		   SWF_warn("WARNING: this might be wrong (%s:%i)\n", __FILE__, __LINE__);
		   puts("if ( ");
		   puts(getName(pop()));
		   println(" ) {");
		   decompileActions(sact->numActions, sact->Actions,gIndent+1);
		}
		else	/* while(){}  as usual */
		{
			puts("while( ");
			puts(getName(pop()));
			println(" ) {");
			offSave=offseoloop;
			offseoloop=actions[n+1].SWF_ACTIONRECORD.Offset;
			decompileActions(sact->numActions-1, sact->Actions,gIndent+1);
			offseoloop=offSave;
		}
		INDENT
		println("}");
		return 0;
	}
	{ // WTF ???
#define SOME_IF_DEBUG 0	/* coders only */
		int has_else_or_break= ((sact->Actions[sact->numActions-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_JUMP) &&
			(sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset > 0 )) ? 1:0;
		int has_lognot=(OpCode(actions, n-1, maxn) == SWFACTION_LOGICALNOT) ? 1:0;
		int else_action_cnt=0,is_logor=0,is_logand=0,sbi,sbe;

		/* before emitting any "if"/"else" characters let's check 
			for a ternary operation  cond?a:b 
		*/
		if (has_else_or_break)
		{
			int limit=actions[n+1].SWF_ACTIONRECORD.Offset + sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset;
			/* Count the number of action records that are part of
			 * the else clause, and then decompile only that many.
			 */
			for(else_action_cnt=0;
			    else_action_cnt+n+1<maxn && actions[n+1+else_action_cnt].SWF_ACTIONRECORD.Offset < limit;
			    else_action_cnt++)
			{
#if SOME_IF_DEBUG
				println("/* ELSE OP 0x%x at %d*/", OpCode(actions, n+1+else_action_cnt, maxn),
				actions[n+1+else_action_cnt].SWF_ACTIONRECORD.Offset)
#endif
				;
			}  
		}
		i=else_action_cnt; 		// =return value
		sbi=stackVal (sact->numActions-1,sact->Actions);
		sbe=stackVal (else_action_cnt,&actions[n+1]);

		// check against opcodes we do not expect in a ternary operation
		if (sbi==1 && sbe==1)
	   	{
			for (j=0;j<sact->numActions-1;j++)
			{
				if (sact->Actions[j].SWF_ACTIONRECORD.ActionCode==SWFACTION_JUMP) // perhaps more ops
				{
					sbi=i=has_else_or_break=0;
					break;
				}
			}
			for (j=0;j<else_action_cnt;j++)
			{
				if (OpCode(actions, n+j, maxn) == SWFACTION_JUMP) // perhaps more ops
				{
					sbe=i=has_else_or_break=0;
					break;
				}
			}
		}
#if SOME_IF_DEBUG
		printf("sbi=%d   sbe=%d\n", sbi,sbe);
#endif
		if (sbi==1 && sbe==1)
		{
#if SOME_IF_DEBUG
			println("/* ****Found ternary ternary operation  \"cond ? a : b\"    **** */");
			printf("If   Actions=%d\n",sact->numActions-1);
			printf("Else Actions=%d\n",else_action_cnt);
#endif
			struct strbufinfo origbuf;
#if USE_LIB
			origbuf=setTempString();	/* switch to a temporary string buffer */
#endif
			puts("(");
			puts(getName(pop()));
			puts(" ? ");
			decompileActions(else_action_cnt  , &actions[n+1],0);
			puts(getName(pop()));
			puts(" : ");
			decompileActions(sact->numActions-1, sact->Actions,0);
			puts(getName(pop()));
			puts(")");
#if USE_LIB
			push (newVar(dcgetstr()));	/* push for later assignment */
			setOrigString(origbuf);		/* switch back to orig buffer */
#else
			push (newVar("/* ternary op: see code above */"));
#endif
		} 
		else
		{
		/* at this point let's check for conditioned jumps that are NOT 'if':
	      	currently that is code for the locical operations  && and ||
	   	*/
			if (OpCode(actions, n-1, maxn) == SWFACTION_PUSHDUP)
				is_logor=1;
			
			if (OpCode(actions, n-2, maxn)== SWFACTION_PUSHDUP
			    && OpCode(actions, n-1, maxn) == SWFACTION_LOGICALNOT)
			{
				is_logand=1;
			}

		if (is_logor || is_logand)    
		{
#if SOME_IF_DEBUG
			println("");
			println("/* detected LOGICAL %s: %d actions*/", is_logor ? "OR":"AND",sact->numActions);
#endif
#if USE_LIB
			origbuf=setTempString();	/* switch to a temporary string buffer */
#endif

			puts(getName(pop()));	/* get left side of logical or */
			puts(is_logor ? " || ":" && ");
			decompileActions(sact->numActions, sact->Actions,gIndent+1);
			puts(getName(pop()));	/* get right side of logical or */
#if USE_LIB
			push(newVar(dcgetstr()));
			setOrigString(origbuf);	/* switch back to orig buffer */
#else
			push (newVar("/* see logical term lines above */")); 
#endif
			return 0;
		}
#ifdef DECOMP_SWITCH
		if ( OpCode(actions, n-1, maxn) == SWFACTION_STRICTEQUALS
		     && check_switch(sact->Actions[0].SWF_ACTIONRECORD.ActionCode) )
		{
			union SWF_ACTION *xact,*xact0;
			for(i=n-1,j=0; i< maxn ;i++)	// n-1 due adding 1st SWFACTION_STRICTEQUALS in buffer	
			{
				j+=countAllSwitchActions(&actions[i],&actions[i-1]); 		// FIRST count size of code
			}
			xact0=xact = (union SWF_ACTION *) calloc (j,sizeof (SWF_ACTION));
			INDENT
			println("// checking %d actions for switch(){}",j);
			for(i=n-1; i< maxn ;i++)
			{
				xact=getAllSwitchActions(xact,&actions[i],&actions[i-1]);	// SECOND copy into xtra buffer
			}
			j=decompile_SWITCH(0,xact0,j,actions[n+1].SWF_ACTIONRECORD.Offset);	// THIRD decompile xtra buffer
			free(xact0);
			return j;
		}
#endif
		/* it seems we have a found the REAL 'if' statement,
		so it's right time to print the "if" just NOW!
		*/
		INDENT
		puts("if( ");
		puts(getName(pop()));	/* the condition itself */
		println(" ) {");
		if ( has_else_or_break )
		{
			int limit=actions[n+1].SWF_ACTIONRECORD.Offset + sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset;
			// limit == dest of jmp == offset next op after 'if' + jumpdist at end of 'if'
			int lastopsize=actions[maxn-1].SWF_ACTIONRECORD.Length;
			if (OpCode(actions, maxn-1, maxn) == SWFACTION_IF)
				lastopsize+=actions[maxn-1].SWF_ACTIONIF.BranchOffset + 3; /* +3 see parser.c: "Action + Length bytes not included in the length" */
			
			if (offseoloop 
			    &&  ! (has_lognot
			    && OpCode(actions, n-2, maxn) == SWFACTION_EQUALS2 
			    && OpCode(actions, n-3, maxn) == SWFACTION_PUSH
			    && OpCode(actions, n-4, maxn) == SWFACTION_PUSHDUP)
			    && limit > actions[maxn-1].SWF_ACTIONRECORD.Offset+lastopsize)
			{
				/* the jump leads outside this limit, so it is a simple 'if'
				with a 'break' or 'return' at the end, and there is NO else clause.
				*/  
				INDENT
				println("// offs_endjump_dest=%d  offs_after_blk %d",
				        limit, actions[maxn-1].SWF_ACTIONRECORD.Offset+lastopsize);
				decompileActions(sact->numActions, sact->Actions,gIndent+1);
				i=0;			/* found break/return but no else and thus return 0 */
			}
			else
			{
				/* There is an else clause also! 
				(action counter is set above)
				*/
				struct _stack *StackSave=Stack;	/* decompile if and else blocks at same stack base */
				if  (has_lognot)
				{
					decompileActions(sact->numActions-1, sact->Actions,gIndent+1);
					INDENT
					println("} else {");
				}	      
				Stack=StackSave;
				decompileActions(else_action_cnt  , &actions[n+1],gIndent+1);
				if  (!has_lognot)		/* the missing if-part just NOW */
				{
					Stack=StackSave;
					INDENT
					println ("} else {" );
					decompileActions(sact->numActions-1, sact->Actions,gIndent+1);
				}
			}
		} 
		else 
		{
			/* It's a simple if() {} */
			decompileActions(sact->numActions, sact->Actions,gIndent+1);
		}
		INDENT
		println("}");
	} // WTF ???
	return i;
	}
	return 0;
}