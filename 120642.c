GF_Err tfhd_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_TrackFragmentHeaderBox *ptr = (GF_TrackFragmentHeaderBox *)s;

	ptr->trackID = gf_bs_read_u32(bs);

	//The rest depends on the flags
	if (ptr->flags & GF_ISOM_TRAF_BASE_OFFSET) {
		ptr->base_data_offset = gf_bs_read_u64(bs);
	}
	if (ptr->flags & GF_ISOM_TRAF_SAMPLE_DESC) {
		ptr->sample_desc_index = gf_bs_read_u32(bs);
	}
	if (ptr->flags & GF_ISOM_TRAF_SAMPLE_DUR) {
		ptr->def_sample_duration = gf_bs_read_u32(bs);
	}
	if (ptr->flags & GF_ISOM_TRAF_SAMPLE_SIZE) {
		ptr->def_sample_size = gf_bs_read_u32(bs);
	}
	if (ptr->flags & GF_ISOM_TRAF_SAMPLE_FLAGS) {
		ptr->def_sample_flags = gf_bs_read_u32(bs);
	}
	return GF_OK;
}