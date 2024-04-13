static int smack_msg_msg_alloc_security(struct msg_msg *msg)
{
	struct smack_known **blob = smack_msg_msg(msg);

	*blob = smk_of_current();
	return 0;
}