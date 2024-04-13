static void xen_crash_shutdown(struct pt_regs *regs)
{
	xen_reboot(SHUTDOWN_crash);
}