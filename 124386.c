dcputs(const char *s)
{
	int len=strlen(s);
	dcchkstr(len);
	strcat(dcptr,s);
	dcptr+=len;
	strsize+=len;
}