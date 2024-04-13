static int smack_file_alloc_security(struct file *file)
{
	struct smack_known **blob = smack_file(file);

	*blob = smk_of_current();
	return 0;
}