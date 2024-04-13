GF_Err tfxd_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_MSSTimeExtBox *ptr = (GF_MSSTimeExtBox *)s;
	if (ptr->size<4) return GF_ISOM_INVALID_FILE;
	ptr->version = gf_bs_read_u8(bs);
	ptr->flags = gf_bs_read_u24(bs);
	ISOM_DECREASE_SIZE(ptr, 4);

	if (ptr->version == 0x01) {
		ptr->absolute_time_in_track_timescale = gf_bs_read_u64(bs);
		ptr->fragment_duration_in_track_timescale = gf_bs_read_u64(bs);
	} else {
		ptr->absolute_time_in_track_timescale = gf_bs_read_u32(bs);
		ptr->fragment_duration_in_track_timescale = gf_bs_read_u32(bs);
	}

	return GF_OK;
}