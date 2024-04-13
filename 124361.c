newVar(char *var)
{
	struct SWF_ACTIONPUSHPARAM *v;

	v=malloc(sizeof(struct SWF_ACTIONPUSHPARAM));
	v->Type = PUSH_VARIABLE; 
	v->p.String = var;
	return v;
}