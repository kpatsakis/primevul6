static void xen_machine_halt(void)
{
	xen_reboot(SHUTDOWN_poweroff);
}