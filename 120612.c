GF_Err free_Read(GF_Box *s, GF_BitStream *bs)
{
	u32 bytesToRead;
	GF_FreeSpaceBox *ptr = (GF_FreeSpaceBox *)s;

	if (ptr->size > 0xFFFFFFFF) return GF_IO_ERR;

	bytesToRead = (u32) (ptr->size);

	if (bytesToRead) {
		ptr->data = (char*)gf_malloc(bytesToRead * sizeof(char));
		gf_bs_read_data(bs, ptr->data, bytesToRead);
		ptr->dataSize = bytesToRead;
	}
	return GF_OK;
}