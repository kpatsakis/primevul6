static int smack_task_movememory(struct task_struct *p)
{
	return smk_curacc_on_task(p, MAY_WRITE, __func__);
}