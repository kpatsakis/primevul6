newVar5(char *var,char *var2, char *var3,char *var4,char *var5)
{
	struct SWF_ACTIONPUSHPARAM *v;

	v=malloc(sizeof(struct SWF_ACTIONPUSHPARAM));
	v->Type = PUSH_VARIABLE; /* VARIABLE */
	v->p.String = malloc(strlen(var)+strlen(var2)+strlen(var3)+strlen(var4)+strlen(var5)+1);
	strcpy(v->p.String,var);
	strcat(v->p.String,var2);
	strcat(v->p.String,var3);
	strcat(v->p.String,var4);
	strcat(v->p.String,var5);
	return v;
}