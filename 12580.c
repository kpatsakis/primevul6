static int selinux_task_getpgid(struct task_struct *p)
{
	return avc_has_perm(&selinux_state,
			    current_sid(), task_sid_obj(p), SECCLASS_PROCESS,
			    PROCESS__GETPGID, NULL);
}