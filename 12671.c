static int smack_task_setscheduler(struct task_struct *p)
{
	return smk_curacc_on_task(p, MAY_WRITE, __func__);
}