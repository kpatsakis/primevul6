static void selinux_task_getsecid_subj(struct task_struct *p, u32 *secid)
{
	*secid = task_sid_subj(p);
}