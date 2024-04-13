static int smack_task_setnice(struct task_struct *p, int nice)
{
	return smk_curacc_on_task(p, MAY_WRITE, __func__);
}