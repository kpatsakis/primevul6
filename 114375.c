GF_DecoderConfig *gf_isom_get_decoder_config(GF_ISOFile *the_file, u32 trackNumber, u32 StreamDescriptionIndex)
{
	GF_TrackBox *trak;
	GF_ESD *esd;
	GF_Descriptor *decInfo;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return NULL;
	//get the ESD (possibly emulated)
	Media_GetESD(trak->Media, StreamDescriptionIndex, &esd, GF_FALSE);
	if (!esd) return NULL;
	decInfo = (GF_Descriptor *) esd->decoderConfig;
	esd->decoderConfig = NULL;
	gf_odf_desc_del((GF_Descriptor *) esd);
	return (GF_DecoderConfig *)decInfo;
}