decompileCAST(int n, SWF_ACTION *actions, int maxn)
{
	struct SWF_ACTIONPUSHPARAM *iparam=pop();
	struct SWF_ACTIONPUSHPARAM *tparam=pop();
	push(newVar_N( getName(tparam),"(",getName(iparam),"", 0,")")); 
	return 0;
}