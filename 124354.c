decompileINITOBJECT(int n, SWF_ACTION *actions, int maxn)
{
	struct SWF_ACTIONPUSHPARAM *nparam;
	nparam=pop();
	push(newVar_N2("","","","{", nparam->p.Integer,"}"));
	return 0;
}