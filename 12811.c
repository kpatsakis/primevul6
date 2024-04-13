static int smack_task_setioprio(struct task_struct *p, int ioprio)
{
	return smk_curacc_on_task(p, MAY_WRITE, __func__);
}