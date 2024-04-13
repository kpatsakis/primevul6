static int smack_ismaclabel(const char *name)
{
	return (strcmp(name, XATTR_SMACK_SUFFIX) == 0);
}