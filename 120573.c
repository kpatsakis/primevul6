
GF_Err fiin_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	FDItemInformationBox *ptr = (FDItemInformationBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u16(bs, gf_list_count(ptr->partition_entries) );
	e = gf_isom_box_array_write(s, ptr->partition_entries, bs);
	if (e) return e;
	if (ptr->session_info) gf_isom_box_write((GF_Box*)ptr->session_info, bs);
	if (ptr->group_id_to_name) gf_isom_box_write((GF_Box*)ptr->group_id_to_name, bs);
	return GF_OK;