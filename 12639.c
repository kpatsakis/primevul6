static int smack_inode_setsecurity(struct inode *inode, const char *name,
				   const void *value, size_t size, int flags)
{
	struct smack_known *skp;
	struct inode_smack *nsp = smack_inode(inode);
	struct socket_smack *ssp;
	struct socket *sock;
	int rc = 0;

	if (value == NULL || size > SMK_LONGLABEL || size == 0)
		return -EINVAL;

	skp = smk_import_entry(value, size);
	if (IS_ERR(skp))
		return PTR_ERR(skp);

	if (strcmp(name, XATTR_SMACK_SUFFIX) == 0) {
		nsp->smk_inode = skp;
		nsp->smk_flags |= SMK_INODE_INSTANT;
		return 0;
	}
	/*
	 * The rest of the Smack xattrs are only on sockets.
	 */
	if (inode->i_sb->s_magic != SOCKFS_MAGIC)
		return -EOPNOTSUPP;

	sock = SOCKET_I(inode);
	if (sock == NULL || sock->sk == NULL)
		return -EOPNOTSUPP;

	ssp = sock->sk->sk_security;

	if (strcmp(name, XATTR_SMACK_IPIN) == 0)
		ssp->smk_in = skp;
	else if (strcmp(name, XATTR_SMACK_IPOUT) == 0) {
		ssp->smk_out = skp;
		if (sock->sk->sk_family == PF_INET) {
			rc = smack_netlbl_add(sock->sk);
			if (rc != 0)
				printk(KERN_WARNING
					"Smack: \"%s\" netlbl error %d.\n",
					__func__, -rc);
		}
	} else
		return -EOPNOTSUPP;

#ifdef SMACK_IPV6_PORT_LABELING
	if (sock->sk->sk_family == PF_INET6)
		smk_ipv6_port_label(sock, NULL);
#endif

	return 0;
}