GF_Err stbl_Size(GF_Box *s)
{
	GF_Err e;
	GF_SampleTableBox *ptr = (GF_SampleTableBox *)s;

	//Mandatory boxs (but not internally :)
	if (ptr->SampleDescription) {
		e = gf_isom_box_size((GF_Box *) ptr->SampleDescription);
		if (e) return e;
		ptr->size += ptr->SampleDescription->size;
	}
	if (ptr->SampleSize) {
		e = gf_isom_box_size((GF_Box *) ptr->SampleSize);
		if (e) return e;
		ptr->size += ptr->SampleSize->size;
	}
	if (ptr->SampleToChunk) {
		e = gf_isom_box_size((GF_Box *) ptr->SampleToChunk);
		if (e) return e;
		ptr->size += ptr->SampleToChunk->size;
	}
	if (ptr->TimeToSample) {
		e = gf_isom_box_size((GF_Box *) ptr->TimeToSample);
		if (e) return e;
		ptr->size += ptr->TimeToSample->size;
	}
	if (ptr->ChunkOffset) {
		e = gf_isom_box_size(ptr->ChunkOffset);
		if (e) return e;
		ptr->size += ptr->ChunkOffset->size;
	}

	//optional boxs
	if (ptr->CompositionOffset)	{
		e = gf_isom_box_size((GF_Box *) ptr->CompositionOffset);
		if (e) return e;
		ptr->size += ptr->CompositionOffset->size;
	}
	if (ptr->CompositionToDecode)	{
		e = gf_isom_box_size((GF_Box *) ptr->CompositionToDecode);
		if (e) return e;
		ptr->size += ptr->CompositionToDecode->size;
	}
	if (ptr->DegradationPriority) {
		e = gf_isom_box_size((GF_Box *) ptr->DegradationPriority);
		if (e) return e;
		ptr->size += ptr->DegradationPriority->size;
	}
	if (ptr->ShadowSync) {
		e = gf_isom_box_size((GF_Box *) ptr->ShadowSync);
		if (e) return e;
		ptr->size += ptr->ShadowSync->size;
	}
	if (ptr->SyncSample) {
		e = gf_isom_box_size((GF_Box *) ptr->SyncSample);
		if (e) return e;
		ptr->size += ptr->SyncSample->size;
	}
	if (ptr->SampleDep && ptr->SampleDep->sampleCount) {
		e = gf_isom_box_size((GF_Box *) ptr->SampleDep);
		if (e) return e;
		ptr->size += ptr->SampleDep->size;
	}
	//padb
	if (ptr->PaddingBits) {
		e = gf_isom_box_size((GF_Box *) ptr->PaddingBits);
		if (e) return e;
		ptr->size += ptr->PaddingBits->size;
	}

	if (ptr->sub_samples) {
		e = gf_isom_box_array_size(s, ptr->sub_samples);
		if (e) return e;
	}
	if (ptr->sampleGroups) {
		e = gf_isom_box_array_size(s, ptr->sampleGroups);
		if (e) return e;
	}
	if (ptr->sampleGroupsDescription) {
		e = gf_isom_box_array_size(s, ptr->sampleGroupsDescription);
		if (e) return e;
	}
	if (ptr->sai_sizes) {
		e = gf_isom_box_array_size(s, ptr->sai_sizes);
		if (e) return e;
	}
	if (ptr->sai_offsets) {
		e = gf_isom_box_array_size(s, ptr->sai_offsets);
		if (e) return e;
	}
	return GF_OK;
}