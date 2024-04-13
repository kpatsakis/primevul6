
GF_Err strk_Size(GF_Box *s)
{
	GF_Err e;
	GF_SubTrackBox *ptr = (GF_SubTrackBox *)s;

	if (ptr->info) {
		e = gf_isom_box_size((GF_Box *)ptr->info);
		if (e) return e;
		ptr->size += ptr->info->size;
	}
	return GF_OK;