GF_Err mfro_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_MovieFragmentRandomAccessOffsetBox *ptr = (GF_MovieFragmentRandomAccessOffsetBox *)s;

	ptr->container_size = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, 4);

	return GF_OK;
}