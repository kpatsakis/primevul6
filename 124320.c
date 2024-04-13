newVar3(char *var,char *var2, char *var3)
{
	struct SWF_ACTIONPUSHPARAM *v;

	v=malloc(sizeof(struct SWF_ACTIONPUSHPARAM));
	v->Type = PUSH_VARIABLE; /* VARIABLE */
	v->p.String = malloc(strlen(var)+strlen(var2)+strlen(var3)+1);
	strcpy(v->p.String,var);
	strcat(v->p.String,var2);
	strcat(v->p.String,var3);
	return v;
}