
GF_Err void_Write(GF_Box *s, GF_BitStream *bs)
{
	gf_bs_write_u32(bs, 0);
	return GF_OK;