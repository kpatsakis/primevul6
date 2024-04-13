static void setOrigString(struct strbufinfo old)
{
	free(dcstr);				/* not needed anymore */
	strsize=old.size;
	strmaxsize=old.maxsize;
	dcstr=old.str;
	dcptr=old.ptr;
}