GF_Err moof_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_MovieFragmentBox *ptr = (GF_MovieFragmentBox *) s;
	if (!s) return GF_BAD_PARAM;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	//Header First
	if (ptr->mfhd) {
		e = gf_isom_box_write((GF_Box *) ptr->mfhd, bs);
		if (e) return e;
	}
//then the track list
return gf_isom_box_array_write(s, ptr->TrackList, bs);
}