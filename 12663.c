static int smack_task_kill(struct task_struct *p, struct kernel_siginfo *info,
			   int sig, const struct cred *cred)
{
	struct smk_audit_info ad;
	struct smack_known *skp;
	struct smack_known *tkp = smk_of_task_struct_obj(p);
	int rc;

	if (!sig)
		return 0; /* null signal; existence test */

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_TASK);
	smk_ad_setfield_u_tsk(&ad, p);
	/*
	 * Sending a signal requires that the sender
	 * can write the receiver.
	 */
	if (cred == NULL) {
		rc = smk_curacc(tkp, MAY_DELIVER, &ad);
		rc = smk_bu_task(p, MAY_DELIVER, rc);
		return rc;
	}
	/*
	 * If the cred isn't NULL we're dealing with some USB IO
	 * specific behavior. This is not clean. For one thing
	 * we can't take privilege into account.
	 */
	skp = smk_of_task(smack_cred(cred));
	rc = smk_access(skp, tkp, MAY_DELIVER, &ad);
	rc = smk_bu_note("USB signal", skp, tkp, MAY_DELIVER, rc);
	return rc;
}