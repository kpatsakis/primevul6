GF_Err tmax_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_TMAXBox *ptr = (GF_TMAXBox *)s;
	ptr->maxTime = gf_bs_read_u32(bs);
	return GF_OK;
}