MaybeStartWalReceiver(void)
{
	if (WalReceiverPID == 0 &&
		(pmState == PM_STARTUP || pmState == PM_RECOVERY ||
		 pmState == PM_HOT_STANDBY) &&
		Shutdown <= SmartShutdown)
	{
		WalReceiverPID = StartWalReceiver();
		if (WalReceiverPID != 0)
			WalReceiverRequested = false;
		/* else leave the flag set, so we'll try again later */
	}
}