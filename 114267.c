static GF_MovieFragmentBox *gf_isom_get_moof(GF_ISOFile *file, u32 moof_index)
{
	u32 i;
	for (i=0; i<gf_list_count(file->TopBoxes); i++) {
		GF_Box *a = (GF_Box*)gf_list_get(file->TopBoxes, i);
		if (a->type==GF_ISOM_BOX_TYPE_MOOF) {
			moof_index--;
			if (!moof_index) return (GF_MovieFragmentBox *) a;
		}
	}
	return NULL;
}