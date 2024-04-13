static int smack_ptrace_traceme(struct task_struct *ptp)
{
	int rc;
	struct smack_known *skp;

	skp = smk_of_task(smack_cred(current_cred()));

	rc = smk_ptrace_rule_check(ptp, skp, PTRACE_MODE_ATTACH, __func__);
	return rc;
}