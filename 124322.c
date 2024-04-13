decompileGETMEMBER(int n, SWF_ACTION *actions, int maxn)
{
	struct SWF_ACTIONPUSHPARAM *obj, *mem, *var;
	char *vname, *varname,*memname;
	int len;

	mem=pop();
	var=pop();
	varname=getName(var);
	memname=getName(mem);
#ifdef DEBUG
	printf("*getMember* varName %s (type=%d)  memName=%s (type=%d)\n",
	       varname,var->Type, memname,mem->Type);
#endif
	len = strlen(varname)+strlen(memname);
	if (mem->Type == PUSH_INT || mem->Type == PUSH_DOUBLE || mem->Type == PUSH_VARIABLE
	    || mem->Type == PUSH_REGISTER || mem->Type == 12 )
	{
		vname = malloc(len+3);
		strcpy(vname,varname);
		strcat(vname,"[");
		strcat(vname,memname);
		strcat(vname,"]");
	}
	else
	{
		vname = malloc(len+2);
		strcpy(vname,varname);
		strcat(vname,".");
		strcat(vname,memname);
	} 
	obj = newVar(vname);
	pushvar(obj);

	return 0;
}