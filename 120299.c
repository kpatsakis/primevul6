GF_Err mehd_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_MovieExtendsHeaderBox *ptr = (GF_MovieExtendsHeaderBox *)s;

	if (ptr->version==1) {
		ptr->fragment_duration = gf_bs_read_u64(bs);
	} else {
		ptr->fragment_duration = (u64) gf_bs_read_u32(bs);
	}
	return GF_OK;
}