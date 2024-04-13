GF_Err stbl_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	//we need to parse DegPrior in a special way
	GF_SampleTableBox *ptr = (GF_SampleTableBox *)s;

	e = gf_isom_box_array_read(s, bs, stbl_AddBox);
	if (e) return e;

	if (!ptr->SyncSample)
		ptr->no_sync_found = 1;

	ptr->nb_sgpd_in_stbl = gf_list_count(ptr->sampleGroupsDescription);
	ptr->nb_other_boxes_in_stbl = gf_list_count(ptr->other_boxes);

	return GF_OK;
}