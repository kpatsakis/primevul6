
GF_Err trak_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_TrackBox *ptr = (GF_TrackBox *)s;

	if (ptr->sample_encryption && ptr->sample_encryption->load_needed) {
		if (!ptr->moov || !!ptr->moov->mov || !ptr->moov->mov->movieFileMap)
			return GF_ISOM_INVALID_FILE;
		GF_Err e = senc_Parse(ptr->moov->mov->movieFileMap->bs, ptr, NULL, ptr->sample_encryption);
		if (e) return e;
	}

	gf_isom_check_position(s, (GF_Box *)ptr->Header, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->Aperture, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->References, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->editBox, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->Media, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->meta, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->groups, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->udta, &pos);
	return GF_OK;