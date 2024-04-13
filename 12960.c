static void selinux_secmark_refcount_dec(void)
{
	atomic_dec(&selinux_secmark_refcount);
}