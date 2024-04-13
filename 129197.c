long asmlinkage compat_sys_nfsservctl(int cmd, void *notused, void *notused2)
{
	return sys_ni_syscall();
}