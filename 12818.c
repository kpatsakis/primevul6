static void smack_ipc_getsecid(struct kern_ipc_perm *ipp, u32 *secid)
{
	struct smack_known **blob = smack_ipc(ipp);
	struct smack_known *iskp = *blob;

	*secid = iskp->smk_secid;
}