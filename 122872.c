void gf_bs_write_u64(GF_BitStream *bs, u64 value)
{
	assert(!bs->nbBits);
	gf_bs_write_u32(bs, (u32) ((value>>32)&0xffffffff));
	gf_bs_write_u32(bs, (u32) (value&0xffffffff));
}