static int smack_kernel_act_as(struct cred *new, u32 secid)
{
	struct task_smack *new_tsp = smack_cred(new);

	new_tsp->smk_task = smack_from_secid(secid);
	return 0;
}