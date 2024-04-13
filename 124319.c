static char* strcatext(char *dest,char *src)
{
	char *r=dest;
	while (*dest)
		dest++;
	strcpyext(dest,src);
	return r;
}