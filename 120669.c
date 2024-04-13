GF_Err tmin_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_TMINBox *ptr = (GF_TMINBox *)s;
	ptr->minTime = gf_bs_read_u32(bs);
	return GF_OK;
}