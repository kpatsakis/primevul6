
GF_Err fdsa_AddBox(GF_Box *s, GF_Box *a)
{
	GF_HintSample *ptr = (GF_HintSample *)s;
	switch(a->type) {
	case GF_ISOM_BOX_TYPE_FDPA:
		gf_list_add(ptr->packetTable, a);
		break;
	case GF_ISOM_BOX_TYPE_EXTR:
		if (ptr->extra_data) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->extra_data = (GF_ExtraDataBox*)a;
		break;
	default:
		return gf_isom_box_add_default(s, a);
	}
	return GF_OK;