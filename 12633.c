static void selinux_task_getsecid_obj(struct task_struct *p, u32 *secid)
{
	*secid = task_sid_obj(p);
}