decompileGETPROPERTY(int n, SWF_ACTION *actions,int maxn)
{
	struct SWF_ACTIONPUSHPARAM *idx, *obj;

	INDENT
	idx = pop();
	obj = pop();
#ifdef DEBUG
	printf("*GETProp* objName %s (type=%d) Prop (type=%d) =%x\n",
	       getName(obj), obj->Type, idx->Type,getInt(idx));
#endif
	if (obj->Type == PUSH_VARIABLE)
		push( newVar5("eval(",getName(obj),".",getProperty(getInt(idx)),")"));
	else
		push( newVar3( getName(obj),".",getProperty(getInt(idx))));
	return 0;
}