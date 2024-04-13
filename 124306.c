decompile_Null_ArgBuiltInFunctionCall(int n, SWF_ACTION *actions, int maxn, char *functionname)
{
	INDENT
	puts(functionname);		// only used for cases w/o return value
	println("();" );
	return 0;
}