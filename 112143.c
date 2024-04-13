ShmemBackendArrayRemove(Backend *bn)
{
	int			i = bn->child_slot - 1;

	Assert(ShmemBackendArray[i].pid == bn->pid);
	/* Mark the slot as empty */
	ShmemBackendArray[i].pid = 0;
}