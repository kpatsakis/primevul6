GF_Err dmax_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_DMAXBox *ptr = (GF_DMAXBox *)s;
	ptr->maxDur = gf_bs_read_u32(bs);
	return GF_OK;
}