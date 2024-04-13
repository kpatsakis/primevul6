
GF_Err metx_AddBox(GF_Box *s, GF_Box *a)
{
	GF_MetaDataSampleEntryBox *ptr = (GF_MetaDataSampleEntryBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_SINF:
		gf_list_add(ptr->protections, a);
		break;
	case GF_ISOM_BOX_TYPE_TXTC:
		//we allow the config box on metx
		if (ptr->config) ERROR_ON_DUPLICATED_BOX(a, ptr)
			ptr->config = (GF_TextConfigBox *)a;
		break;
	default:
		return gf_isom_box_add_default(s, a);
	}
	return GF_OK;