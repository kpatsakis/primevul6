
GF_Err extr_Size(GF_Box *s)
{
	GF_Err e;
	GF_ExtraDataBox *ptr = (GF_ExtraDataBox *) s;

	if (ptr->feci) {
		e = gf_isom_box_size((GF_Box *)ptr->feci);
		if (e) return e;
		ptr->size += ptr->feci->size;
	}
	ptr->size += ptr->data_length;
	return GF_OK;