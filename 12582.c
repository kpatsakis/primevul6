static int smack_task_setpgid(struct task_struct *p, pid_t pgid)
{
	return smk_curacc_on_task(p, MAY_WRITE, __func__);
}