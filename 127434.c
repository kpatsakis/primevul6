bool xen_set_default_idle(void)
{
	bool ret = !!x86_idle;

	x86_idle = default_idle;

	return ret;
}