static void smack_free_mnt_opts(void *mnt_opts)
{
	struct smack_mnt_opts *opts = mnt_opts;
	kfree(opts->fsdefault);
	kfree(opts->fsfloor);
	kfree(opts->fshat);
	kfree(opts->fsroot);
	kfree(opts->fstransmute);
	kfree(opts);
}