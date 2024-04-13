GF_Err mvex_Size(GF_Box *s)
{
	GF_Err e;
	GF_MovieExtendsBox *ptr = (GF_MovieExtendsBox *)s;

	if (ptr->mehd) {
		e = gf_isom_box_size((GF_Box *)ptr->mehd);
		if (e) return e;
		ptr->size += ptr->mehd->size;
	}
	e = gf_isom_box_array_size(s, ptr->TrackExList);
	if (e) return e;
	return gf_isom_box_array_size(s, ptr->TrackExPropList);
}