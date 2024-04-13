static int selinux_binder_transfer_binder(struct task_struct *from,
					  struct task_struct *to)
{
	return avc_has_perm(&selinux_state,
			    task_sid_binder(from), task_sid_binder(to),
			    SECCLASS_BINDER, BINDER__TRANSFER,
			    NULL);
}