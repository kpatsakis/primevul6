u8 gf_isom_is_track_in_root_od(GF_ISOFile *movie, u32 trackNumber)
{
	u32 i;
	GF_ISOTrackID trackID;
	GF_Descriptor *desc;
	GF_ES_ID_Inc *inc;
	GF_List *inc_list;
	if (!movie) return 2;
	if (!movie->moov || !movie->moov->iods) return 0;

	desc = movie->moov->iods->descriptor;
	switch (desc->tag) {
	case GF_ODF_ISOM_IOD_TAG:
		inc_list = ((GF_IsomInitialObjectDescriptor *)desc)->ES_ID_IncDescriptors;
		break;
	case GF_ODF_ISOM_OD_TAG:
		inc_list = ((GF_IsomObjectDescriptor *)desc)->ES_ID_IncDescriptors;
		break;
	//files without IOD are possible !
	default:
		return 0;
	}
	trackID = gf_isom_get_track_id(movie, trackNumber);
	if (!trackID) return 2;
	i=0;
	while ((inc = (GF_ES_ID_Inc*)gf_list_enum(inc_list, &i))) {
		if (inc->trackID == (u32) trackID) return 1;
	}
	return 0;
}