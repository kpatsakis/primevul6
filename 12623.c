static int smack_shm_shmat(struct kern_ipc_perm *isp, char __user *shmaddr,
			   int shmflg)
{
	int may;

	may = smack_flags_to_may(shmflg);
	return smk_curacc_shm(isp, may);
}