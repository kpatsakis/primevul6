GF_Err srpp_Size(GF_Box *s)
{
	GF_Err e;
	GF_SRTPProcessBox *ptr = (GF_SRTPProcessBox *)s;

	s->size += 16;
	if (ptr->info) {
		e = gf_isom_box_size((GF_Box*)ptr->info);
		if (e) return e;
		ptr->size += ptr->info->size;
	}
	if (ptr->scheme_type) {
		e = gf_isom_box_size((GF_Box*)ptr->scheme_type);
		if (e) return e;
		ptr->size += ptr->scheme_type->size;
	}
	return GF_OK;
}