static int smack_inode_getsecurity(struct user_namespace *mnt_userns,
				   struct inode *inode, const char *name,
				   void **buffer, bool alloc)
{
	struct socket_smack *ssp;
	struct socket *sock;
	struct super_block *sbp;
	struct inode *ip = (struct inode *)inode;
	struct smack_known *isp;

	if (strcmp(name, XATTR_SMACK_SUFFIX) == 0)
		isp = smk_of_inode(inode);
	else {
		/*
		 * The rest of the Smack xattrs are only on sockets.
		 */
		sbp = ip->i_sb;
		if (sbp->s_magic != SOCKFS_MAGIC)
			return -EOPNOTSUPP;

		sock = SOCKET_I(ip);
		if (sock == NULL || sock->sk == NULL)
			return -EOPNOTSUPP;

		ssp = sock->sk->sk_security;

		if (strcmp(name, XATTR_SMACK_IPIN) == 0)
			isp = ssp->smk_in;
		else if (strcmp(name, XATTR_SMACK_IPOUT) == 0)
			isp = ssp->smk_out;
		else
			return -EOPNOTSUPP;
	}

	if (alloc) {
		*buffer = kstrdup(isp->smk_known, GFP_KERNEL);
		if (*buffer == NULL)
			return -ENOMEM;
	}

	return strlen(isp->smk_known);
}