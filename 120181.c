GF_Err pmax_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_PMAXBox *ptr = (GF_PMAXBox *)s;
	ptr->maxSize = gf_bs_read_u32(bs);
	return GF_OK;
}