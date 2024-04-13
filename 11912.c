open_tempfile(char *filename)
{
    FILE *result = 0;

    _nc_STRCPY(filename, "/tmp/XXXXXX", PATH_MAX);
#if HAVE_MKSTEMP
    {
	int oldmask = (int) umask(077);
	int fd = mkstemp(filename);
	if (fd >= 0)
	    result = fdopen(fd, "w");
	umask((mode_t) oldmask);
    }
#else
    if (tmpnam(filename) != 0)
	result = fopen(filename, "w");
#endif
    return result;
}