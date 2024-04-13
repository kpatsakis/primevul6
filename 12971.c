static void init_task_smack(struct task_smack *tsp, struct smack_known *task,
					struct smack_known *forked)
{
	tsp->smk_task = task;
	tsp->smk_forked = forked;
	INIT_LIST_HEAD(&tsp->smk_rules);
	INIT_LIST_HEAD(&tsp->smk_relabel);
	mutex_init(&tsp->smk_rules_lock);
}