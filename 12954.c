static int smack_msg_queue_associate(struct kern_ipc_perm *isp, int msqflg)
{
	int may;

	may = smack_flags_to_may(msqflg);
	return smk_curacc_msq(isp, may);
}