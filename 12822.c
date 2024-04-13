static int smack_shm_associate(struct kern_ipc_perm *isp, int shmflg)
{
	int may;

	may = smack_flags_to_may(shmflg);
	return smk_curacc_shm(isp, may);
}