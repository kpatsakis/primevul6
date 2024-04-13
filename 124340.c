decompileSETPROPERTY(int n, SWF_ACTION *actions,int maxn)
{
	struct SWF_ACTIONPUSHPARAM *val, *idx, *obj;

	INDENT
	val = pop();
	idx = pop();
	obj = pop();
#ifdef DEBUG
	printf("*setProp* objName %s (type=%d) Prop (type=%d) =%x\n",
	       getName(obj), obj->Type, idx->Type,getInt(idx));
#endif
	if (obj->Type == PUSH_VARIABLE)
		puts("eval(");
	
	decompilePUSHPARAM(obj,0);
	if (obj->Type == PUSH_VARIABLE)
		puts(")");
	
	puts(".");
	puts(getProperty(getInt(idx)));
	printf(" = " );
	decompilePUSHPARAM(val,0);
	println(";");
	return 0;
}