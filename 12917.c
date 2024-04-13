static int smack_msg_queue_msgctl(struct kern_ipc_perm *isp, int cmd)
{
	int may;

	switch (cmd) {
	case IPC_STAT:
	case MSG_STAT:
	case MSG_STAT_ANY:
		may = MAY_READ;
		break;
	case IPC_SET:
	case IPC_RMID:
		may = MAY_READWRITE;
		break;
	case IPC_INFO:
	case MSG_INFO:
		/*
		 * System level information
		 */
		return 0;
	default:
		return -EINVAL;
	}

	return smk_curacc_msq(isp, may);
}