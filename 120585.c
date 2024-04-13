GF_Box *stsd_New()
{
	ISOM_DECL_BOX_ALLOC(GF_SampleDescriptionBox, GF_ISOM_BOX_TYPE_STSD);
	tmp->other_boxes = gf_list_new();
	return (GF_Box *)tmp;
}