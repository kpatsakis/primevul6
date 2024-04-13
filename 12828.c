static int smack_msg_queue_msgrcv(struct kern_ipc_perm *isp, struct msg_msg *msg,
			struct task_struct *target, long type, int mode)
{
	return smk_curacc_msq(isp, MAY_READWRITE);
}