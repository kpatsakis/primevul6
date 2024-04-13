newVar_N(char *var,char *var2, char *var3,char *var4,int pop_counter,char *final)
{
	struct SWF_ACTIONPUSHPARAM *v;
	int psize=PARAM_STRSIZE;
	int i;
	int slen=strlen(var)+strlen(var2)+strlen(var3)+strlen(var4)+strlen(final);
	
	v=malloc(sizeof(struct SWF_ACTIONPUSHPARAM));
	v->p.String = malloc(psize + slen);
	v->Type = PUSH_VARIABLE; 
	strcpy(v->p.String,var);
	strcat(v->p.String,var2);
	strcat(v->p.String,var3);
	strcat(v->p.String,var4);
	for(i=0;i<pop_counter;i++) 
	{
		char *pops=getString(pop());
		while ( strlen(v->p.String)+ 2 + strlen(pops) +slen >= psize)
		{
			psize += PARAM_STRSIZE;
			v->p.String = realloc( v->p.String, psize);
		}
		strcat(v->p.String,pops);
		if( i < pop_counter-1 ) 
			strcat(v->p.String,",");
	}
	strcat(v->p.String,final);
	return v;
}