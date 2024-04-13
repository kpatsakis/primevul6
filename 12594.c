static int smack_msg_queue_msgsnd(struct kern_ipc_perm *isp, struct msg_msg *msg,
				  int msqflg)
{
	int may;

	may = smack_flags_to_may(msqflg);
	return smk_curacc_msq(isp, may);
}