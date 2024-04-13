static int smack_cred_alloc_blank(struct cred *cred, gfp_t gfp)
{
	init_task_smack(smack_cred(cred), NULL, NULL);
	return 0;
}