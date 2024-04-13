decompileArithmeticOp(int n, SWF_ACTION *actions, int maxn)
{
	struct SWF_ACTIONPUSHPARAM *left, *right;
	int op_l = OpCode(actions, n, maxn);
	int op_r = OpCode(actions, n+1, maxn);
	right=pop();
	left=pop();
	switch(OpCode(actions, n, maxn))
	{
	/*
	case SWFACTION_GETMEMBER:
		decompilePUSHPARAM(peek(),0);
		break;
	*/
	case SWFACTION_INSTANCEOF:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left)," instanceof ",getString(right)));
		else
			push(newVar_N("(",getString(left)," instanceof ",getString(right),0,")"));
		break;
	case SWFACTION_ADD:
	case SWFACTION_ADD2:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),"+",getString(right)));
		else
			push(newVar_N("(",getString(left),"+",getString(right),0,")"));
		break;
	case SWFACTION_SUBTRACT:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),"-",getString(right)));	      
		else
			push(newVar_N("(",getString(left),"-",getString(right),0,")"));
		break;
	case SWFACTION_MULTIPLY:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),"*",getString(right)));
		else
			push(newVar_N("(",getString(left),"*",getString(right),0,")"));
		break;
	case SWFACTION_DIVIDE:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),"/",getString(right)));
		else
			push(newVar_N("(",getString(left),"/",getString(right),0,")"));
		break;
	case SWFACTION_MODULO:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),"%",getString(right)));
		else
			push(newVar_N("(",getString(left),"%",getString(right),0,")"));
		break;
	case SWFACTION_SHIFTLEFT:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),"<<",getString(right)));
		else
			push(newVar_N("(",getString(left),"<<",getString(right),0,")"));
		break;
	case SWFACTION_SHIFTRIGHT:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),">>",getString(right)));
		else
			push(newVar_N("(",getString(left),">>",getString(right),0,")"));
		break;
	case SWFACTION_SHIFTRIGHT2:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),">>>",getString(right)));
		else
			push(newVar_N("(",getString(left),">>>",getString(right),0,")"));
		break;
	case SWFACTION_LOGICALAND:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),"&&",getString(right)));
		else
			push(newVar_N("(",getString(left),"&&",getString(right),0,")"));
		break;
	case SWFACTION_LOGICALOR:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),"||",getString(right)));
		else
			push(newVar_N("(",getString(left),"||",getString(right),0,")"));
		break;
	case SWFACTION_BITWISEAND:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),"&",getString(right)));
		else
			push(newVar_N("(",getString(left),"&",getString(right),0,")"));
		break;
	case SWFACTION_BITWISEOR:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),"|",getString(right)));
		else
			push(newVar_N("(",getString(left),"|",getString(right),0,")"));
		break;
	case SWFACTION_BITWISEXOR:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),"^",getString(right)));
		else
			push(newVar_N("(",getString(left),"^",getString(right),0,")"));
		break;
	case SWFACTION_EQUALS2:	/* including negation */
	case SWFACTION_EQUAL:
		if( OpCode(actions, n+1, maxn) == SWFACTION_LOGICALNOT &&
		    OpCode(actions, n+2, maxn) != SWFACTION_IF)
		{
			op_r = OpCode(actions, n+1, maxn);
			if (precedence(op_l, op_r))
				push(newVar3(getString(left),"!=",getString(right)));
			else
				push(newVar_N("(",getString(left),"!=",getString(right),0,")"));
			return 1; /* due negation op */
		}
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),"==",getString(right)));
		else
			push(newVar_N("(",getString(left),"==",getString(right),0,")"));
		break;
	case SWFACTION_LESS2:
		if( OpCode(actions, n+1, maxn)  == SWFACTION_LOGICALNOT &&
		    OpCode(actions, n+2, maxn) != SWFACTION_IF ) 
		{
			op_r = OpCode(actions, n+2, maxn);
			if (precedence(op_l, op_r))
				push(newVar3(getString(left),">=",getString(right)));
			else
				push(newVar_N("(",getString(left),">=",getString(right),0,")"));
			return 1; /* due negation op */
		}
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),"<",getString(right)));
		else
			push(newVar_N("(",getString(left),"<",getString(right),0,")"));
		break;
	case SWFACTION_GREATER:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),">",getString(right)));
		else
			push(newVar_N("(",getString(left),">",getString(right),0,")"));
		break;
	case SWFACTION_LESSTHAN:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),"<",getString(right)));
		else
			push(newVar_N("(",getString(left),"<",getString(right),0,")"));
		break;
	case SWFACTION_STRINGEQ:
		if (precedence(op_l, op_r))
			push(newVar3(getString(left),"==",getString(right)));
		else
			push(newVar_N("(",getString(left),"==",getString(right),0,")"));
		break;
	case SWFACTION_STRINGCOMPARE:
		puts("STRINGCOMPARE");
		break;
	case SWFACTION_STRICTEQUALS:
#ifdef DECOMP_SWITCH
		if  (OpCode(actions, n, maxn) == SWFACTION_IF)
		{
			int code = actions[n+1].SWF_ACTIONIF.Actions[0].SWF_ACTIONRECORD.ActionCode;
			if(check_switch(code))
			{
				push(right);	// keep left and right side separated
				push(left);	// because it seems we have found a switch(){} and
				break;	// let decompileIF() once more do all the dirty work
			}
		}
#endif
		if( OpCode(actions, n+1, maxn) == SWFACTION_LOGICALNOT &&
		    OpCode(actions, n+2, maxn) != SWFACTION_IF ) 
		{
			op_r = OpCode(actions, n+2, maxn);
			if (precedence(op_l, op_r))
				push(newVar3(getString(left),"!==",getString(right)));
			else
				push(newVar_N("(",getString(left),"!==",getString(right),0,")"));
			return 1; /* due negation op */
		} else {
			if (precedence(op_l, op_r))
				push(newVar3(getString(left),"===",getString(right)));
			else
				push(newVar_N("(",getString(left),"===",getString(right),0,")"));
			break;
		}
	default:
		printf("Unhandled Arithmetic/Logic OP %x\n",
			OpCode(actions, n, maxn));
	}
	return 0;
}