static int smack_inode_init_security(struct inode *inode, struct inode *dir,
				     const struct qstr *qstr, const char **name,
				     void **value, size_t *len)
{
	struct inode_smack *issp = smack_inode(inode);
	struct smack_known *skp = smk_of_current();
	struct smack_known *isp = smk_of_inode(inode);
	struct smack_known *dsp = smk_of_inode(dir);
	int may;

	if (name)
		*name = XATTR_SMACK_SUFFIX;

	if (value && len) {
		rcu_read_lock();
		may = smk_access_entry(skp->smk_known, dsp->smk_known,
				       &skp->smk_rules);
		rcu_read_unlock();

		/*
		 * If the access rule allows transmutation and
		 * the directory requests transmutation then
		 * by all means transmute.
		 * Mark the inode as changed.
		 */
		if (may > 0 && ((may & MAY_TRANSMUTE) != 0) &&
		    smk_inode_transmutable(dir)) {
			isp = dsp;
			issp->smk_flags |= SMK_INODE_CHANGED;
		}

		*value = kstrdup(isp->smk_known, GFP_NOFS);
		if (*value == NULL)
			return -ENOMEM;

		*len = strlen(isp->smk_known);
	}

	return 0;
}