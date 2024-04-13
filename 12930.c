static void smack_file_set_fowner(struct file *file)
{
	struct smack_known **blob = smack_file(file);

	*blob = smk_of_current();
}