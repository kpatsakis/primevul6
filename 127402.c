static void xen_machine_power_off(void)
{
	if (pm_power_off)
		pm_power_off();
	xen_reboot(SHUTDOWN_poweroff);
}