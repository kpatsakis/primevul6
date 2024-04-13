void stbl_del(GF_Box *s)
{
	GF_SampleTableBox *ptr = (GF_SampleTableBox *)s;
	if (ptr == NULL) return;

	if (ptr->ChunkOffset) gf_isom_box_del(ptr->ChunkOffset);
	if (ptr->CompositionOffset) gf_isom_box_del((GF_Box *) ptr->CompositionOffset);
	if (ptr->CompositionToDecode) gf_isom_box_del((GF_Box *) ptr->CompositionToDecode);
	if (ptr->DegradationPriority) gf_isom_box_del((GF_Box *) ptr->DegradationPriority);
	if (ptr->SampleDescription) gf_isom_box_del((GF_Box *) ptr->SampleDescription);
	if (ptr->SampleSize) gf_isom_box_del((GF_Box *) ptr->SampleSize);
	if (ptr->SampleToChunk) gf_isom_box_del((GF_Box *) ptr->SampleToChunk);
	if (ptr->ShadowSync) gf_isom_box_del((GF_Box *) ptr->ShadowSync);
	if (ptr->SyncSample) gf_isom_box_del((GF_Box *) ptr->SyncSample);
	if (ptr->TimeToSample) gf_isom_box_del((GF_Box *) ptr->TimeToSample);
	if (ptr->SampleDep) gf_isom_box_del((GF_Box *) ptr->SampleDep);
	if (ptr->PaddingBits) gf_isom_box_del((GF_Box *) ptr->PaddingBits);
	if (ptr->sub_samples) gf_isom_box_array_del(ptr->sub_samples);
	if (ptr->sampleGroups) gf_isom_box_array_del(ptr->sampleGroups);
	if (ptr->sampleGroupsDescription) gf_isom_box_array_del(ptr->sampleGroupsDescription);

	if (ptr->sai_sizes) gf_isom_box_array_del(ptr->sai_sizes);
	if (ptr->sai_offsets) gf_isom_box_array_del(ptr->sai_offsets);
	if (ptr->traf_map) {
		if (ptr->traf_map->sample_num) gf_free(ptr->traf_map->sample_num);
		gf_free(ptr->traf_map);
	}

	gf_free(ptr);
}