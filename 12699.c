static int selinux_task_setpgid(struct task_struct *p, pid_t pgid)
{
	return avc_has_perm(&selinux_state,
			    current_sid(), task_sid_obj(p), SECCLASS_PROCESS,
			    PROCESS__SETPGID, NULL);
}