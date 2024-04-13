BackgroundWorkerUnblockSignals(void)
{
	PG_SETMASK(&UnBlockSig);
}