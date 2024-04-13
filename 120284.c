GF_Err mvex_AddBox(GF_Box *s, GF_Box *a)
{
	GF_MovieExtendsBox *ptr = (GF_MovieExtendsBox *)s;

	switch (a->type) {
	case GF_ISOM_BOX_TYPE_TREX:
		return gf_list_add(ptr->TrackExList, a);
	case GF_ISOM_BOX_TYPE_TREP:
		return gf_list_add(ptr->TrackExPropList, a);
	case GF_ISOM_BOX_TYPE_MEHD:
		if (ptr->mehd) break;
		ptr->mehd = (GF_MovieExtendsHeaderBox*)a;
		return GF_OK;
	default:
		return gf_isom_box_add_default(s, a);
	}
	return GF_OK;
}