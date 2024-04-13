GF_Err gf_isom_freeze_order(GF_ISOFile *file)
{
	u32 i=0;
	GF_Box *box;
	if (!file) return GF_BAD_PARAM;
	while ((box=gf_list_enum(file->TopBoxes, &i))) {
		gf_isom_box_freeze_order(box);
	}
	return GF_OK;
}