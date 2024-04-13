GF_Err padb_Read(GF_Box *s,GF_BitStream *bs)
{
	u32 i;
	GF_PaddingBitsBox *ptr = (GF_PaddingBitsBox *)s;

	ptr->SampleCount = gf_bs_read_u32(bs);

	ptr->padbits = (u8 *)gf_malloc(sizeof(u8)*ptr->SampleCount);
	for (i=0; i<ptr->SampleCount; i += 2) {
		gf_bs_read_int(bs, 1);
		if (i+1 < ptr->SampleCount) {
			ptr->padbits[i+1] = gf_bs_read_int(bs, 3);
		} else {
			gf_bs_read_int(bs, 3);
		}
		gf_bs_read_int(bs, 1);
		ptr->padbits[i] = gf_bs_read_int(bs, 3);
	}
	return GF_OK;
}