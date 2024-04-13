static int smack_mmap_file(struct file *file,
			   unsigned long reqprot, unsigned long prot,
			   unsigned long flags)
{
	struct smack_known *skp;
	struct smack_known *mkp;
	struct smack_rule *srp;
	struct task_smack *tsp;
	struct smack_known *okp;
	struct inode_smack *isp;
	struct superblock_smack *sbsp;
	int may;
	int mmay;
	int tmay;
	int rc;

	if (file == NULL)
		return 0;

	if (unlikely(IS_PRIVATE(file_inode(file))))
		return 0;

	isp = smack_inode(file_inode(file));
	if (isp->smk_mmap == NULL)
		return 0;
	sbsp = smack_superblock(file_inode(file)->i_sb);
	if (sbsp->smk_flags & SMK_SB_UNTRUSTED &&
	    isp->smk_mmap != sbsp->smk_root)
		return -EACCES;
	mkp = isp->smk_mmap;

	tsp = smack_cred(current_cred());
	skp = smk_of_current();
	rc = 0;

	rcu_read_lock();
	/*
	 * For each Smack rule associated with the subject
	 * label verify that the SMACK64MMAP also has access
	 * to that rule's object label.
	 */
	list_for_each_entry_rcu(srp, &skp->smk_rules, list) {
		okp = srp->smk_object;
		/*
		 * Matching labels always allows access.
		 */
		if (mkp->smk_known == okp->smk_known)
			continue;
		/*
		 * If there is a matching local rule take
		 * that into account as well.
		 */
		may = smk_access_entry(srp->smk_subject->smk_known,
				       okp->smk_known,
				       &tsp->smk_rules);
		if (may == -ENOENT)
			may = srp->smk_access;
		else
			may &= srp->smk_access;
		/*
		 * If may is zero the SMACK64MMAP subject can't
		 * possibly have less access.
		 */
		if (may == 0)
			continue;

		/*
		 * Fetch the global list entry.
		 * If there isn't one a SMACK64MMAP subject
		 * can't have as much access as current.
		 */
		mmay = smk_access_entry(mkp->smk_known, okp->smk_known,
					&mkp->smk_rules);
		if (mmay == -ENOENT) {
			rc = -EACCES;
			break;
		}
		/*
		 * If there is a local entry it modifies the
		 * potential access, too.
		 */
		tmay = smk_access_entry(mkp->smk_known, okp->smk_known,
					&tsp->smk_rules);
		if (tmay != -ENOENT)
			mmay &= tmay;

		/*
		 * If there is any access available to current that is
		 * not available to a SMACK64MMAP subject
		 * deny access.
		 */
		if ((may | mmay) != mmay) {
			rc = -EACCES;
			break;
		}
	}

	rcu_read_unlock();

	return rc;
}