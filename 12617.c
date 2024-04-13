static int smack_inode_permission(struct inode *inode, int mask)
{
	struct superblock_smack *sbsp = smack_superblock(inode->i_sb);
	struct smk_audit_info ad;
	int no_block = mask & MAY_NOT_BLOCK;
	int rc;

	mask &= (MAY_READ|MAY_WRITE|MAY_EXEC|MAY_APPEND);
	/*
	 * No permission to check. Existence test. Yup, it's there.
	 */
	if (mask == 0)
		return 0;

	if (sbsp->smk_flags & SMK_SB_UNTRUSTED) {
		if (smk_of_inode(inode) != sbsp->smk_root)
			return -EACCES;
	}

	/* May be droppable after audit */
	if (no_block)
		return -ECHILD;
	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_INODE);
	smk_ad_setfield_u_fs_inode(&ad, inode);
	rc = smk_curacc(smk_of_inode(inode), mask, &ad);
	rc = smk_bu_inode(inode, mask, rc);
	return rc;
}