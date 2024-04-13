dcprintf(char *format, ...)
{
	char *s;
	size_t size;
	int ret;

	va_list args;
	va_start(args,format);
	ret = vasprintf(&s,format,args);
	dcputs(s);
	size=strlen(s);
	free(s);
	return size;
}