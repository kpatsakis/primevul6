u32 gf_bs_read_vluimsbf5(GF_BitStream *bs)
{
	u32 nb_words = 0;
	while (gf_bs_read_int(bs, 1)) nb_words++;
	nb_words++;
	return gf_bs_read_int(bs, 4*nb_words);
}