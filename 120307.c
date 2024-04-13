
void fiin_del(GF_Box *s)
{
	FDItemInformationBox *ptr = (FDItemInformationBox *)s;
	if (ptr == NULL) return;
	if (ptr->partition_entries) gf_isom_box_array_del(ptr->partition_entries);
	if (ptr->session_info) gf_isom_box_del((GF_Box*)ptr->session_info);
	if (ptr->group_id_to_name) gf_isom_box_del((GF_Box*)ptr->group_id_to_name);
	gf_free(ptr);