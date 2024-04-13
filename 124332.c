dcchkstr(int size)
{
	while( (strsize+size) > strmaxsize ) {
		dcstr=realloc(dcstr,strmaxsize+DCSTRSIZE);
		strmaxsize+=DCSTRSIZE;
		dcptr=dcstr+strsize;
	}

}