static int smack_sem_associate(struct kern_ipc_perm *isp, int semflg)
{
	int may;

	may = smack_flags_to_may(semflg);
	return smk_curacc_sem(isp, may);
}