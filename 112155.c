ShmemBackendArrayAllocation(void)
{
	Size		size = ShmemBackendArraySize();

	ShmemBackendArray = (Backend *) ShmemAlloc(size);
	/* Mark all slots as empty */
	memset(ShmemBackendArray, 0, size);
}