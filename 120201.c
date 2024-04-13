/*this is using chpl format according to some NeroRecode samples*/
GF_Err tfdt_Read(GF_Box *s,GF_BitStream *bs)
{
	GF_TFBaseMediaDecodeTimeBox *ptr = (GF_TFBaseMediaDecodeTimeBox *)s;

	if (ptr->version==1) {
		ptr->baseMediaDecodeTime = gf_bs_read_u64(bs);
		ISOM_DECREASE_SIZE(ptr, 8);
	} else {
		ptr->baseMediaDecodeTime = (u32) gf_bs_read_u32(bs);
		ISOM_DECREASE_SIZE(ptr, 4);
	}
	return GF_OK;