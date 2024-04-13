static void selinux_release_secctx(char *secdata, u32 seclen)
{
	kfree(secdata);
}