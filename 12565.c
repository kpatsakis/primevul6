static int selinux_inode_listsecurity(struct inode *inode, char *buffer, size_t buffer_size)
{
	const int len = sizeof(XATTR_NAME_SELINUX);

	if (!selinux_initialized(&selinux_state))
		return 0;

	if (buffer && len <= buffer_size)
		memcpy(buffer, XATTR_NAME_SELINUX, len);
	return len;
}