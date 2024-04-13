static void smack_cred_transfer(struct cred *new, const struct cred *old)
{
	struct task_smack *old_tsp = smack_cred(old);
	struct task_smack *new_tsp = smack_cred(new);

	new_tsp->smk_task = old_tsp->smk_task;
	new_tsp->smk_forked = old_tsp->smk_task;
	mutex_init(&new_tsp->smk_rules_lock);
	INIT_LIST_HEAD(&new_tsp->smk_rules);

	/* cbs copy rule list */
}