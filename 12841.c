static int smack_sem_semop(struct kern_ipc_perm *isp, struct sembuf *sops,
			   unsigned nsops, int alter)
{
	return smk_curacc_sem(isp, MAY_READWRITE);
}