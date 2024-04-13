#ifndef GPAC_DISABLE_ISOM_WRITE
GF_Err sgpd_Write(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_SampleGroupDescriptionBox *p = (GF_SampleGroupDescriptionBox *)s;
	GF_Err e;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, p->grouping_type);
	if (p->version>=1) gf_bs_write_u32(bs, p->default_length);
	if (p->version>=2) gf_bs_write_u32(bs, p->default_description_index);
	gf_bs_write_u32(bs, gf_list_count(p->group_descriptions) );

	for (i=0; i<gf_list_count(p->group_descriptions); i++) {
		void *ptr = gf_list_get(p->group_descriptions, i);
		if ((p->version >= 1) && !p->default_length) {
			u32 size = sgpd_size_entry(p->grouping_type, ptr);
			gf_bs_write_u32(bs, size);
		}
		sgpd_write_entry(p->grouping_type, ptr, bs);
	}
	return GF_OK;