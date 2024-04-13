static int __init init_misc_binfmt(void)
{
	int err = register_filesystem(&bm_fs_type);
	if (!err) {
		err = register_binfmt(&misc_format);
		if (err)
			unregister_filesystem(&bm_fs_type);
	}
	return err;
}