static int smack_inode_copy_up_xattr(const char *name)
{
	/*
	 * Return 1 if this is the smack access Smack attribute.
	 */
	if (strcmp(name, XATTR_NAME_SMACK) == 0)
		return 1;

	return -EOPNOTSUPP;
}