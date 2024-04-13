switchToOrigString(struct strbufinfo old)
{
	char *tmp=dcstr;
	strsize=old.size;
	strmaxsize=old.maxsize;
	dcstr=old.str;
	dcptr=old.ptr;
	return tmp;
}