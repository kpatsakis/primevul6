GF_VVCConfig *gf_isom_vvc_config_get(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex)
{
	GF_TrackBox *trak;
	u32 type;
	GF_MPEGVisualSampleEntryBox *entry;
	/*todo, add support for subpic track and nvcl tracks*/
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !DescriptionIndex) return NULL;
	type = gf_isom_get_vvc_type(the_file, trackNumber, DescriptionIndex);
	if (type==GF_ISOM_VVCTYPE_NONE)
		return NULL;
	entry = (GF_MPEGVisualSampleEntryBox*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, DescriptionIndex-1);
	if (!entry) return NULL;
	if (entry->internal_type != GF_ISOM_SAMPLE_ENTRY_VIDEO) return NULL;

	if ((type==GF_ISOM_VVCTYPE_SUBPIC) || (type==GF_ISOM_VVCTYPE_NVCL)) {
		GF_VVCNaluConfigurationBox *vvnc = (GF_VVCNaluConfigurationBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_VVNC);
		if (!vvnc) return NULL;
		GF_VVCConfig *vvcc = gf_odf_vvc_cfg_new();
		vvcc->nal_unit_size = vvnc->nal_unit_size;
		return vvcc;
	}
	if (!entry->vvc_config) return NULL;
	return VVC_DuplicateConfig(entry->vvc_config->config);
}