static int smack_file_send_sigiotask(struct task_struct *tsk,
				     struct fown_struct *fown, int signum)
{
	struct smack_known **blob;
	struct smack_known *skp;
	struct smack_known *tkp = smk_of_task(smack_cred(tsk->cred));
	const struct cred *tcred;
	struct file *file;
	int rc;
	struct smk_audit_info ad;

	/*
	 * struct fown_struct is never outside the context of a struct file
	 */
	file = container_of(fown, struct file, f_owner);

	/* we don't log here as rc can be overriden */
	blob = smack_file(file);
	skp = *blob;
	rc = smk_access(skp, tkp, MAY_DELIVER, NULL);
	rc = smk_bu_note("sigiotask", skp, tkp, MAY_DELIVER, rc);

	rcu_read_lock();
	tcred = __task_cred(tsk);
	if (rc != 0 && smack_privileged_cred(CAP_MAC_OVERRIDE, tcred))
		rc = 0;
	rcu_read_unlock();

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_TASK);
	smk_ad_setfield_u_tsk(&ad, tsk);
	smack_log(skp->smk_known, tkp->smk_known, MAY_DELIVER, rc, &ad);
	return rc;
}