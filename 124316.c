newVar2(char *var,char *var2)
{
	struct SWF_ACTIONPUSHPARAM *v;

	v=malloc(sizeof(struct SWF_ACTIONPUSHPARAM));
	v->Type = PUSH_VARIABLE;
	v->p.String = malloc(strlen(var)+strlen(var2)+1);
	strcpy(v->p.String,var);
	strcat(v->p.String,var2);
	return v;
}