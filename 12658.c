static int smack_task_getscheduler(struct task_struct *p)
{
	return smk_curacc_on_task(p, MAY_READ, __func__);
}