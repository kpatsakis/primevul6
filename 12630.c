static int selinux_task_setioprio(struct task_struct *p, int ioprio)
{
	return avc_has_perm(&selinux_state,
			    current_sid(), task_sid_obj(p), SECCLASS_PROCESS,
			    PROCESS__SETSCHED, NULL);
}