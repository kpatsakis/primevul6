GF_Err free_Size(GF_Box *s)
{
	GF_FreeSpaceBox *ptr = (GF_FreeSpaceBox *)s;
	ptr->size += ptr->dataSize;
	return GF_OK;
}