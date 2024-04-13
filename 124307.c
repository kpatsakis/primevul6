dcputchar(char c)
{
	dcchkstr(1);

	*dcptr++=c;
	*dcptr='\000';
	strsize++;
}