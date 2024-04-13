static int precedence(int op1,int op2)
{
	static unsigned char ops[]= { 		// array of opcodes w rising precedence
//	SWFACTION_SETVARIABLE,		// TAKE CARE: array is incomplete
//	SWFACTION_TRACE,
	// missing ops are considered with low precedence
		SWFACTION_LOGICALOR,
		SWFACTION_LOGICALAND,
		SWFACTION_BITWISEOR,
		SWFACTION_BITWISEXOR,
		SWFACTION_BITWISEAND,
		SWFACTION_STRICTEQUALS,
		SWFACTION_EQUALS2,
		SWFACTION_EQUAL,
		SWFACTION_GREATER,
		SWFACTION_LESSTHAN,
		SWFACTION_LESS2,	
		SWFACTION_SHIFTRIGHT,
		SWFACTION_SHIFTRIGHT2,
		SWFACTION_SHIFTLEFT,
		SWFACTION_ADD,
		SWFACTION_ADD2,
		SWFACTION_SUBTRACT,
		SWFACTION_MODULO,
		SWFACTION_MULTIPLY,
		SWFACTION_DIVIDE,
		SWFACTION_LOGICALNOT,
		SWFACTION_PUSH			// FIXME: need more analysis on code after PUSH
	};
	unsigned char* f=memchr(ops,op1,sizeof(ops));
	unsigned char* s=memchr(ops,op2,sizeof(ops));
#ifdef DEBUG
	printf("1op=%d 2op=%d  result=%d\n",op1,op2,f>s);
	if (!f) printf("opcode=%d NOT in precedence list\n",op1);
	if (!s) printf("opcode=%d NOT in precedence list\n",op2);
#endif
	return f>s;
}