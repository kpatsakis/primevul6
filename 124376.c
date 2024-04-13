decompileNEWOBJECT(int n, SWF_ACTION *actions, int maxn)
{
	struct SWF_ACTIONPUSHPARAM *obj, *nparam;
	obj = pop();
	nparam=pop();
	push(newVar_N("new ","",getName(obj),"(", nparam->p.Integer,")"));
	return 0;
}