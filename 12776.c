static struct smack_known *smack_of_ipc(struct kern_ipc_perm *isp)
{
	struct smack_known **blob = smack_ipc(isp);

	return *blob;
}