decompileJUMP(int n, SWF_ACTION *actions, int maxn)
{
	int i=0,j=0;
	int offSave;
	struct SWF_ACTIONIF *sactif;
	OUT_BEGIN2(SWF_ACTIONJUMP);
	sactif=NULL;

	if(isLogicalOp(n+1, actions, maxn) ||
	   (OpCode(actions, n+1, maxn) == SWFACTION_PUSH && isLogicalOp(n+2, actions, maxn)))
	{
		/* Probably the start of a do {} while(), so skip it */
		return 0;
	}

	/* Probably the end of a switch{}, so skip it */
	if (OpCode(actions, n+1, maxn) == SWFACTION_POP)
		return 1;

	if (OpCode(actions, n+1, maxn) == SWFACTION_JUMP) 
	{
		if (actions[n+1].SWF_ACTIONJUMP.BranchOffset==0)
			return 1;
	}

	for(i=0; n + 1 + i < maxn && (actions[(n+1)+i].SWF_ACTIONRECORD.Offset < (actions[n+1].SWF_ACTIONRECORD.Offset+actions[n ].SWF_ACTIONJUMP.BranchOffset)); i++)
	{
#if 0
		printf("/* for PART3 OP 0x%x */\n",actions[n+1+i].SWF_ACTIONRECORD.ActionCode);
#endif
		; // NOOP
	}

	if (i)
	{
		for (j=0; n+j+i<maxn; j++)
		{
#if 0
			 printf("/* FOR part2 OP 0x%x */\n",actions[n+i+j].SWF_ACTIONRECORD.ActionCode)
			// at least one should push on stack
#endif
	 
			if (OpCode(actions, n+i+j, maxn) == SWFACTION_IF)
			{
				sactif = (struct SWF_ACTIONIF *)&(actions[n+i+j]);
				/* chk whether last jump does lead us back to start of loop */
				if (sactif->Actions[sactif->numActions-1].SWF_ACTIONRECORD.ActionCode==SWFACTION_JUMP
				    && sactif->Actions[sactif->numActions-1].SWF_ACTIONJUMP.BranchOffset+
				    sactif->Actions[sactif->numActions-1].SWF_ACTIONJUMP.Offset==
				    actions[n].SWF_ACTIONRECORD.Offset )
				{
					break;
				}
				else
					sactif=NULL;
			}
		}
	}

	if (sactif)
	{
		INDENT
		puts("while(");
		decompileActions(j-1, &actions[n+1+i], gIndent);
		puts(getName(pop()));
		println("){         /* original FOR loop rewritten to WHILE */");
		offSave=offseoloop;
		if (n+i+j+1<maxn)						// see part2 above
			offseoloop=actions[n+i+j+1].SWF_ACTIONRECORD.Offset;
		else
			offseoloop=actions[n+i+j].SWF_ACTIONRECORD.Offset+5;
		decompileActions(sactif->numActions-1, sactif->Actions,gIndent+1);
		decompileActions(i, &actions[n+1], gIndent+1);
		offseoloop=offSave;
		INDENT
		println("};");
		return i+j; 
	}
	
	if (sact->BranchOffset>0)
	{
		if ( stackVal(n,actions) == 1 && n+1==maxn)
		{	// leaving block @last op with value on stack: a return x;
			return decompileRETURN(n, actions,maxn);
		}
		if (n+2 < maxn && OpCode(actions, n+1, maxn) == SWFACTION_PUSH && 
			actions[n+2].SWF_ACTIONRECORD.Offset == actions[n+1].SWF_ACTIONRECORD.Offset+sact->BranchOffset)
		{
			return 1; 	// jump to short to be a 'break': but an internal jump over a push
		}			// to do: add some control flow analysis
		
		INDENT
		
		if (offseoloop==actions[n].SWF_ACTIONRECORD.Offset+sact->BranchOffset+5)
			puts("break;" );
		else
			puts("return;" );
		
		println("\t\t\t// offs_end_of_loop=%d  offs_jmp_dest=%d",
		        offseoloop, actions[n].SWF_ACTIONRECORD.Offset+sact->BranchOffset+5);
	}
	else
	{
		if (sact->BranchOffset<0)
		{
			INDENT
			println("continue;     /*------*/");
		}
	}
	/* error("Unhandled JUMP"); */
	return 0;
}