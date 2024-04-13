void compute_creds(struct linux_binprm *bprm)
{
	int unsafe;

	if (bprm->e_uid != current->uid)
		suid_keys(current);
	exec_keys(current);

	task_lock(current);
	unsafe = unsafe_exec(current);
	security_bprm_apply_creds(bprm, unsafe);
	task_unlock(current);
	security_bprm_post_apply_creds(bprm);
}