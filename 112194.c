BackgroundWorkerBlockSignals(void)
{
	PG_SETMASK(&BlockSig);
}