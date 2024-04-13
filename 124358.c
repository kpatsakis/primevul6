dcgetstr()
{
	char *ret;
	ret = dcstr;
	dcstr=NULL;
	strmaxsize=0;
	return ret;
}