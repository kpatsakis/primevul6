
GF_Err strk_AddBox(GF_Box *s, GF_Box *a)
{
	GF_SubTrackBox *ptr = (GF_SubTrackBox *)s;
	if (!a) return GF_OK;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_STRI:
		if (ptr->info) ERROR_ON_DUPLICATED_BOX(a, ptr)
			ptr->info = (GF_SubTrackInformationBox *)a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_STRD:
		if (ptr->strd) ERROR_ON_DUPLICATED_BOX(a, ptr)
			ptr->strd = a;
		return GF_OK;
	default:
		return gf_isom_box_add_default(s, a);
	}
	return GF_OK;