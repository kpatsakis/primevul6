u32 gf_isom_segment_get_fragment_count(GF_ISOFile *file)
{
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	if (file) {
		u32 i, count = 0;
		for (i=0; i<gf_list_count(file->TopBoxes); i++) {
			GF_Box *a = (GF_Box*)gf_list_get(file->TopBoxes, i);
			if (a->type==GF_ISOM_BOX_TYPE_MOOF) count++;
		}
		return count;
	}
#endif
	return 0;
}