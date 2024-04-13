static int selinux_sb_mnt_opts_compat(struct super_block *sb, void *mnt_opts)
{
	struct selinux_mnt_opts *opts = mnt_opts;
	struct superblock_security_struct *sbsec = sb->s_security;
	u32 sid;
	int rc;

	/*
	 * Superblock not initialized (i.e. no options) - reject if any
	 * options specified, otherwise accept.
	 */
	if (!(sbsec->flags & SE_SBINITIALIZED))
		return opts ? 1 : 0;

	/*
	 * Superblock initialized and no options specified - reject if
	 * superblock has any options set, otherwise accept.
	 */
	if (!opts)
		return (sbsec->flags & SE_MNTMASK) ? 1 : 0;

	if (opts->fscontext) {
		rc = parse_sid(sb, opts->fscontext, &sid);
		if (rc)
			return 1;
		if (bad_option(sbsec, FSCONTEXT_MNT, sbsec->sid, sid))
			return 1;
	}
	if (opts->context) {
		rc = parse_sid(sb, opts->context, &sid);
		if (rc)
			return 1;
		if (bad_option(sbsec, CONTEXT_MNT, sbsec->mntpoint_sid, sid))
			return 1;
	}
	if (opts->rootcontext) {
		struct inode_security_struct *root_isec;

		root_isec = backing_inode_security(sb->s_root);
		rc = parse_sid(sb, opts->rootcontext, &sid);
		if (rc)
			return 1;
		if (bad_option(sbsec, ROOTCONTEXT_MNT, root_isec->sid, sid))
			return 1;
	}
	if (opts->defcontext) {
		rc = parse_sid(sb, opts->defcontext, &sid);
		if (rc)
			return 1;
		if (bad_option(sbsec, DEFCONTEXT_MNT, sbsec->def_sid, sid))
			return 1;
	}
	return 0;
}