ShmemBackendArraySize(void)
{
	return mul_size(MaxLivePostmasterChildren(), sizeof(Backend));
}