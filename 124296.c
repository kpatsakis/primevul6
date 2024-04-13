static struct strbufinfo setTempString(void)
{
	struct strbufinfo current;
	current.size=strsize;
	current.maxsize=strmaxsize;
	current.str=dcstr;
	current.ptr=dcptr;
	dcinit();
	return current;
}