
GF_Err fdsa_Size(GF_Box *s)
{
	GF_HintSample *ptr = (GF_HintSample*)s;
	GF_Err e;

	 if (ptr->extra_data) {
		e = gf_isom_box_size((GF_Box *)ptr->extra_data);
		if (e) return e;
		ptr->size += ptr->extra_data->size;
	}
	return gf_isom_box_array_size(s, ptr->packetTable);