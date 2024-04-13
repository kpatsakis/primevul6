static int smack_cred_prepare(struct cred *new, const struct cred *old,
			      gfp_t gfp)
{
	struct task_smack *old_tsp = smack_cred(old);
	struct task_smack *new_tsp = smack_cred(new);
	int rc;

	init_task_smack(new_tsp, old_tsp->smk_task, old_tsp->smk_task);

	rc = smk_copy_rules(&new_tsp->smk_rules, &old_tsp->smk_rules, gfp);
	if (rc != 0)
		return rc;

	rc = smk_copy_relabel(&new_tsp->smk_relabel, &old_tsp->smk_relabel,
				gfp);
	return rc;
}