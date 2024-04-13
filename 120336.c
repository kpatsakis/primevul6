GF_Err mfhd_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_MovieFragmentHeaderBox *ptr = (GF_MovieFragmentHeaderBox *)s;
	ptr->sequence_number = gf_bs_read_u32(bs);
	return GF_OK;
}