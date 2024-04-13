static int smack_inode_listsecurity(struct inode *inode, char *buffer,
				    size_t buffer_size)
{
	int len = sizeof(XATTR_NAME_SMACK);

	if (buffer != NULL && len <= buffer_size)
		memcpy(buffer, XATTR_NAME_SMACK, len);

	return len;
}