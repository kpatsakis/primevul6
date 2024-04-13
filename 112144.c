TerminateChildren(int signal)
{
	SignalChildren(signal);
	if (StartupPID != 0)
	{
		signal_child(StartupPID, signal);
		if (signal == SIGQUIT || signal == SIGKILL)
			StartupStatus = STARTUP_SIGNALED;
	}
	if (BgWriterPID != 0)
		signal_child(BgWriterPID, signal);
	if (CheckpointerPID != 0)
		signal_child(CheckpointerPID, signal);
	if (WalWriterPID != 0)
		signal_child(WalWriterPID, signal);
	if (WalReceiverPID != 0)
		signal_child(WalReceiverPID, signal);
	if (AutoVacPID != 0)
		signal_child(AutoVacPID, signal);
	if (PgArchPID != 0)
		signal_child(PgArchPID, signal);
	if (PgStatPID != 0)
		signal_child(PgStatPID, signal);
}