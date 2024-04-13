static __init int ia32_binfmt_init(void)
{ 
	register_sysctl_table(abi_root_table2);
	return 0;
}