
GF_Err reftype_Read(GF_Box *s, GF_BitStream *bs)
{
	u32 bytesToRead;
	u32 i;
	GF_TrackReferenceTypeBox *ptr = (GF_TrackReferenceTypeBox *)s;

	bytesToRead = (u32) (ptr->size);
	if (!bytesToRead) return GF_OK;

	ptr->trackIDCount = (u32) (bytesToRead) / sizeof(u32);
	ptr->trackIDs = (u32 *) gf_malloc(ptr->trackIDCount * sizeof(u32));
	if (!ptr->trackIDs) return GF_OUT_OF_MEM;

	for (i = 0; i < ptr->trackIDCount; i++) {
		ptr->trackIDs[i] = gf_bs_read_u32(bs);
	}
	return GF_OK;