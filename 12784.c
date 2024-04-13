static int selinux_binder_set_context_mgr(struct task_struct *mgr)
{
	return avc_has_perm(&selinux_state,
			    current_sid(), task_sid_binder(mgr), SECCLASS_BINDER,
			    BINDER__SET_CONTEXT_MGR, NULL);
}