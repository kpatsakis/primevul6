static char* strcpyext(char *dest,char *src)
{
	char *r=dest;
	while (*src)
	{
		if (*src=='\'')
		{
			*dest++='\\';
#if BSC == 2
			*dest++='\\';
#endif
		}
		*dest++=*src++;
	}
	*dest='\0';
	return r;
}