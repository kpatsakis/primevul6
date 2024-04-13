GF_ESD *gf_isom_get_esd(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex)
{
	GF_ESD *esd;
	GF_Err e;
	e = GetESD(movie->moov, gf_isom_get_track_id(movie, trackNumber), StreamDescriptionIndex, &esd);
	if (e && (e!= GF_ISOM_INVALID_MEDIA)) {
		movie->LastError = e;
		if (esd) gf_odf_desc_del((GF_Descriptor *)esd);
		return NULL;
	}

	return esd;
}