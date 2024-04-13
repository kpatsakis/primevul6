decompileNEWMETHOD(int n, SWF_ACTION *actions, int maxn)
{
	char *t;
	struct SWF_ACTIONPUSHPARAM *meth, *nparam, *obj;
	meth = pop();
	obj = pop();
	nparam=pop();

	t=malloc(strlen( getName(obj) ) +2);
	strcpy(t,getName(obj));
	strcat(t,".");

	push(newVar_N("new ",t,getName(meth),"(", nparam->p.Integer,")"));
	free (t);
	return 0;
}