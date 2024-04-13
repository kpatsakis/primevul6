static void xen_restart(char *msg)
{
	xen_reboot(SHUTDOWN_reboot);
}