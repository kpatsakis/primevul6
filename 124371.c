dcinit()
{
	strsize = 1; // We start with empty string, i.e. \0
	strmaxsize=DCSTRSIZE;
	dcstr=calloc(DCSTRSIZE,1);
	dcptr=dcstr;
}