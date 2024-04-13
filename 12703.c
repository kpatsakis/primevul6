static int smack_ipc_alloc_security(struct kern_ipc_perm *isp)
{
	struct smack_known **blob = smack_ipc(isp);

	*blob = smk_of_current();
	return 0;
}