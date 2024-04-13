static int selinux_task_setnice(struct task_struct *p, int nice)
{
	return avc_has_perm(&selinux_state,
			    current_sid(), task_sid_obj(p), SECCLASS_PROCESS,
			    PROCESS__SETSCHED, NULL);
}