static char *dirname(char *path)
{
	char *fname;

	fname = basename(path);
	--fname;
	*fname = '\0';
	return path;
}