
GF_Err fiin_AddBox(GF_Box *s, GF_Box *a)
{
	FDItemInformationBox *ptr = (FDItemInformationBox *)s;
	switch(a->type) {
	case GF_ISOM_BOX_TYPE_PAEN:
		if (!ptr->partition_entries) ptr->partition_entries = gf_list_new();
		return gf_list_add(ptr->partition_entries, a);
	case GF_ISOM_BOX_TYPE_SEGR:
		if (ptr->session_info) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->session_info = (FDSessionGroupBox *)a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_GITN:
		if (ptr->group_id_to_name) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->group_id_to_name = (GroupIdToNameBox *)a;
		return GF_OK;
	default:
		return gf_isom_box_add_default(s, a);
	}
	return GF_OK;