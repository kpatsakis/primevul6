static int smack_bprm_creds_for_exec(struct linux_binprm *bprm)
{
	struct inode *inode = file_inode(bprm->file);
	struct task_smack *bsp = smack_cred(bprm->cred);
	struct inode_smack *isp;
	struct superblock_smack *sbsp;
	int rc;

	isp = smack_inode(inode);
	if (isp->smk_task == NULL || isp->smk_task == bsp->smk_task)
		return 0;

	sbsp = smack_superblock(inode->i_sb);
	if ((sbsp->smk_flags & SMK_SB_UNTRUSTED) &&
	    isp->smk_task != sbsp->smk_root)
		return 0;

	if (bprm->unsafe & LSM_UNSAFE_PTRACE) {
		struct task_struct *tracer;
		rc = 0;

		rcu_read_lock();
		tracer = ptrace_parent(current);
		if (likely(tracer != NULL))
			rc = smk_ptrace_rule_check(tracer,
						   isp->smk_task,
						   PTRACE_MODE_ATTACH,
						   __func__);
		rcu_read_unlock();

		if (rc != 0)
			return rc;
	}
	if (bprm->unsafe & ~LSM_UNSAFE_PTRACE)
		return -EPERM;

	bsp->smk_task = isp->smk_task;
	bprm->per_clear |= PER_CLEAR_ON_SETID;

	/* Decide if this is a secure exec. */
	if (bsp->smk_task != bsp->smk_forked)
		bprm->secureexec = 1;

	return 0;
}