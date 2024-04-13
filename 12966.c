static void selinux_secmark_refcount_inc(void)
{
	atomic_inc(&selinux_secmark_refcount);
}