static int smk_bu_current(char *note, struct smack_known *oskp,
			  int mode, int rc)
{
	struct task_smack *tsp = smack_cred(current_cred());
	char acc[SMK_NUM_ACCESS_TYPE + 1];

	if (rc <= 0)
		return rc;
	if (rc > SMACK_UNCONFINED_OBJECT)
		rc = 0;

	smk_bu_mode(mode, acc);
	pr_info("Smack %s: (%s %s %s) %s %s\n", smk_bu_mess[rc],
		tsp->smk_task->smk_known, oskp->smk_known,
		acc, current->comm, note);
	return 0;
}