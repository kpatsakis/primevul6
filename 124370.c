newVar_N2(char *var,char *var2, char *var3,char *var4,int pop_counter,char *final)
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
		char *pops1=getString(pop());
		char *pops2=getName  (pop());

		while ( strlen(v->p.String)+ 3 + strlen(pops1)+ strlen(pops2) +slen >= psize)
		{
			psize += PARAM_STRSIZE;
			v->p.String = realloc( v->p.String, psize);
		}
		strcat(v->p.String,pops2);
		strcat(v->p.String,":");
		strcat(v->p.String,pops1);
		if( i < pop_counter-1 ) 
			strcat(v->p.String,",");
	}
	strcat(v->p.String,final);
	return v;
}