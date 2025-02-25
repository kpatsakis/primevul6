decompile_SWITCH(int n, SWF_ACTION *actions, int maxn, int off1end)
{
	int i,j;
	int start;		// base action index for case value and code
	int ccsize=0;		// size of code for case value
	int cvsize=0;		// size of case value
	int maxoff=0;		// action offset AFTER switch
	int n_maxoff=0;		// array index of maxoff
	int pend=0;		// control pending output
	int xsize=0;		// ret val
	int jmpsize=0;		// debug helper
	int lastoff=0;		// debug helper
	int n_firstactions=maxn;// array index of 1st case actions code
	int lastcasestart=0;	// offs where last "case x:" begins
	char *defa="[last]";	// debug helper for early "default:" 
	char *tmp=NULL;		// helper for pending output
	struct strbufinfo origbuf;	// pending output buffer
	struct _stack *StackSave;
	struct SWF_ACTIONPUSHPARAM *swcopy,*sw=pop();
	struct SWF_ACTIONPUSHPARAM *compare=pop();
	int offSave;
	for (i=0; i<n_firstactions; i++) // seek last op in 1st if
	{
		if (actions[i+1].SWF_ACTIONRECORD.Offset==off1end)
		{
			// println("found #off end first= %d",i+1);
			if (OpCode(actions, i, maxn) == SWFACTION_JUMP)
			{
				maxoff=actions[i].SWF_ACTIONJUMP.BranchOffset+actions[i].SWF_ACTIONJUMP.Offset+5;
				j=1;
			}
			else
			{
				// SanityCheck(decompile_SWITCH,0,"no jump found where expected");
			}
			break;
		} 
	}
	
	if (!maxoff)
	{
		for (i=maxn-1;i>=0;i--)			// seek from end of block last op of switch{}
		{
			if (OpCode(actions, i, maxn) == SWFACTION_JUMP && !actions[i].SWF_ACTIONJUMP.BranchOffset)
			{
				maxoff=actions[i].SWF_ACTIONRECORD.Offset+5;
				j=2;
				break;
			} 
		}
	}

	for (i=0;i<maxn;i++)	
	{
		if (actions[i].SWF_ACTIONRECORD.Offset>=maxoff)
		{
			n_maxoff=i;		// part of block is switch
			break;
		}
	}
		
	if (!n_maxoff) 
		n_maxoff=maxn;			// whole block is switch

	INDENT
	println("switch( %s ) {			// end switch at %d (index %d) / found via meth %d)",
	        getString(sw), maxoff,n_maxoff,j);
		
	push(sw);
	push(compare);

	i=1;
	do 					// here we go into main loop
	{
		if((OpCode(actions, i, maxn) == SWFACTION_IF
		   && OpCode(actions, i-1, maxn) == SWFACTION_STRICTEQUALS )
		   ||(OpCode(actions, i, maxn) == SWFACTION_JUMP
		   && OpCode(actions, i-1, maxn) == SWFACTION_IF) )
		{
			start=i;
			while (start<maxn 
			       && actions[start].SWF_ACTIONRECORD.Offset < actions[i].SWF_ACTIONRECORD.Offset+5+actions[i].SWF_ACTIONJUMP.BranchOffset
)			{
				start++;		// count actions until start of "case x:"
			}
			if (n_firstactions==maxn) // if not done store earliest "case x: "actions
			{
				n_firstactions=start;	// same as array index
			}

			for (ccsize=0; ccsize+start<n_maxoff; ccsize++)	// count actions belonging to "case x:"
			{
#ifdef DEBUGSWITCH
				println("in ccsize: ccsize=%d  off=%d %s",
				        ccsize,actions[ccsize+start].SWF_ACTIONRECORD.Offset,
				        actionName(OpCode(actions, ccsize+start, maxn)));
#endif
				if (OpCode(actions, ccsize+start, maxn) == SWFACTION_JUMP)
				{
					if (maxoff == actions[ccsize+start].SWF_ACTIONJUMP.Offset+5 + actions[ccsize+start].SWF_ACTIONJUMP.BranchOffset)
					{
						jmpsize= actions[ccsize+start].SWF_ACTIONJUMP.BranchOffset;
						lastoff= actions[ccsize+start].SWF_ACTIONJUMP.Offset;
						ccsize++; // the jmp itself
						break;
					}
				}
			}

#if USE_LIB
			if (tmp && (start!=pend)) // output pending buffer if neccessary
			{
				puts(tmp);
			}
			
			if (tmp)
			{
				free(tmp);
				tmp=NULL;
			}
			pend=start;
#endif
			if (OpCode(actions, i, maxn) == SWFACTION_JUMP)
			{
				if (ccsize<=1)
					break;	// ready
				else
				{
					INDENT
					if (actions[start].SWF_ACTIONRECORD.Offset>lastcasestart)
						xsize+=ccsize;        
					else
						defa="[early]";
						println("default:			// at %d %s start=%d ccsize=%d",
						        actions[start].SWF_ACTIONRECORD.Offset,defa, start, ccsize);
				}
			}
			else
			{
				INDENT
				xsize=ccsize;
				lastcasestart=actions[start].SWF_ACTIONRECORD.Offset;
				println("case %s:			// at %d  start=%d ccsize=%d jmp=%d+%d+5",
			        getString(pop()), lastcasestart, start, ccsize, lastoff,jmpsize);
				swcopy=pop();
				//   SanityCheck(decompile_SWITCH,!strcmp(getName(swcopy),getName(sw)),"sw0 != sw");
			}

#if USE_LIB
			origbuf=setTempString(); // switch to temp buffer
#endif
			StackSave=Stack;
			offSave=offseoloop;
			offseoloop=maxoff;
			decompileActions( ccsize, &actions[start],gIndent+1);
			offseoloop=offSave;
			Stack=StackSave;
#if USE_LIB
			tmp=switchToOrigString(origbuf);
#endif

			if (OpCode(actions, i, maxn) == SWFACTION_JUMP)		// after "default:"
			{
				break;     							// ready
			}
			else
			{
				if (OpCode(actions, i+1, maxn) != SWFACTION_JUMP) 	// not before "default:" or end
				{
					i++; // the 'if' itself
					cvsize=0;
					while (i+cvsize < n_firstactions 
					       && OpCode(actions, i+cvsize, maxn) != SWFACTION_STRICTEQUALS)
					{
#ifdef DEBUGSWITCH
						println("in  cvsize=%d  %d %s",
						        cvsize, actions[i+cvsize].SWF_ACTIONRECORD.Offset,
						        actionName(OpCode(actions, i+cvsize, maxn)));
#endif
							cvsize++;	// count "case X:" code size
					}
					decompileActions( cvsize, &actions[i],gIndent+1); // at least one push on stack expected
					i+=cvsize;
				}
			}
		}
	} while (++i < n_firstactions);

#if USE_LIB
	if (tmp)
	{
		puts(tmp);		// print last pending output
		free(tmp);
	}
#endif	
	INDENT
	println("}					// switch ret value =%d",xsize);
	return xsize;
}