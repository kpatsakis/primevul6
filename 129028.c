int copy_strings_kernel(int argc,char ** argv, struct linux_binprm *bprm)
{
	int r;
	mm_segment_t oldfs = get_fs();
	set_fs(KERNEL_DS);
	r = copy_strings(argc, (char __user * __user *)argv, bprm);
	set_fs(oldfs);
	return r;
}