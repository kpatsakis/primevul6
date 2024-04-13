GF_Err tpyl_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_NTYLBox *ptr = (GF_NTYLBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;
	ptr->nbBytes = gf_bs_read_u64(bs);
	return GF_OK;
}