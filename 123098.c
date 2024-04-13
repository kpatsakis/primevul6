static void NEVER_RETURNS _rad_panic(const char *file, unsigned int line,
				    const char *msg)
{
	radlog(L_ERR, "[%s:%d] %s", file, line, msg);
	_exit(1);
}