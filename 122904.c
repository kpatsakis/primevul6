void gf_bs_truncate(GF_BitStream *bs)
{
	bs->size = bs->position;
}