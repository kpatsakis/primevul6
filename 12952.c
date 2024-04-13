static int smk_bu_task(struct task_struct *otp, int mode, int rc)
{
	struct task_smack *tsp = smack_cred(current_cred());
	struct smack_known *smk_task = smk_of_task_struct_obj(otp);
	char acc[SMK_NUM_ACCESS_TYPE + 1];

	if (rc <= 0)
		return rc;
	if (rc > SMACK_UNCONFINED_OBJECT)
		rc = 0;

	smk_bu_mode(mode, acc);
	pr_info("Smack %s: (%s %s %s) %s to %s\n", smk_bu_mess[rc],
		tsp->smk_task->smk_known, smk_task->smk_known, acc,
		current->comm, otp->comm);
	return 0;
}