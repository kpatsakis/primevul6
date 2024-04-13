decompileGETVARIABLE(int n, SWF_ACTION *actions, int maxn)
{
	struct SWF_ACTIONPUSHPARAM *var;

	var = pop();
#ifdef DEBUG
	printf("*GETVariable* varName %s (type=%d)\n",getName(var),var->Type);
#endif
	if (var->Type == PUSH_VARIABLE)
		pushvar(newVar3("eval(",getName(var),")"));
	else
		pushvar(newVar(getName(var)));

	return 0;
}