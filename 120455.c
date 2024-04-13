
GF_Err vmhd_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_VideoMediaHeaderBox *ptr = (GF_VideoMediaHeaderBox *)s;

	ptr->reserved = gf_bs_read_u64(bs);
	return GF_OK;