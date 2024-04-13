GF_Err video_sample_entry_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_MPEGVisualSampleEntryBox *mp4v = (GF_MPEGVisualSampleEntryBox*)s;
	GF_Err e;
	e = gf_isom_video_sample_entry_read((GF_VisualSampleEntryBox *)s, bs);
	if (e) return e;
	e = gf_isom_box_array_read(s, bs, video_sample_entry_AddBox);
	if (e) return e;
	/*this is an AVC sample desc*/
	if (mp4v->avc_config || mp4v->svc_config || mp4v->mvc_config) AVC_RewriteESDescriptor(mp4v);
	/*this is an HEVC sample desc*/
	if (mp4v->hevc_config || mp4v->lhvc_config || (mp4v->type==GF_ISOM_BOX_TYPE_HVT1))
		HEVC_RewriteESDescriptor(mp4v);
	/*this is an AV1 sample desc*/
	if (mp4v->av1_config)
		AV1_RewriteESDescriptor(mp4v);
	return GF_OK;
}