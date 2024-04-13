GF_Err stbl_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_SampleTableBox *ptr = (GF_SampleTableBox *)s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	if (ptr->SampleDescription) {
		e = gf_isom_box_write((GF_Box *) ptr->SampleDescription, bs);
		if (e) return e;
	}
	if (ptr->TimeToSample) {
		e = gf_isom_box_write((GF_Box *) ptr->TimeToSample, bs);
		if (e) return e;
	}
	if (ptr->CompositionOffset)	{
		e = gf_isom_box_write((GF_Box *) ptr->CompositionOffset, bs);
		if (e) return e;
	}
	if (ptr->CompositionToDecode)	{
		e = gf_isom_box_write((GF_Box *) ptr->CompositionToDecode, bs);
		if (e) return e;
	}
	if (ptr->SyncSample) {
		e = gf_isom_box_write((GF_Box *) ptr->SyncSample, bs);
		if (e) return e;
	}
	if (ptr->ShadowSync) {
		e = gf_isom_box_write((GF_Box *) ptr->ShadowSync, bs);
		if (e) return e;
	}
	if (ptr->SampleToChunk) {
		e = gf_isom_box_write((GF_Box *) ptr->SampleToChunk, bs);
		if (e) return e;
	}
	if (ptr->SampleSize) {
		e = gf_isom_box_write((GF_Box *) ptr->SampleSize, bs);
		if (e) return e;
	}
	if (ptr->ChunkOffset) {
		e = gf_isom_box_write(ptr->ChunkOffset, bs);
		if (e) return e;
	}
	if (ptr->DegradationPriority) {
		e = gf_isom_box_write((GF_Box *) ptr->DegradationPriority, bs);
		if (e) return e;
	}
	if (ptr->SampleDep && ptr->SampleDep->sampleCount) {
		e = gf_isom_box_write((GF_Box *) ptr->SampleDep, bs);
		if (e) return e;
	}
	if (ptr->PaddingBits) {
		e = gf_isom_box_write((GF_Box *) ptr->PaddingBits, bs);
		if (e) return e;
	}
	if (ptr->sub_samples) {
		e = gf_isom_box_array_write(s, ptr->sub_samples, bs);
		if (e) return e;
	}
	if (ptr->sampleGroupsDescription) {
		e = gf_isom_box_array_write(s, ptr->sampleGroupsDescription, bs);
		if (e) return e;
	}
	if (ptr->sampleGroups) {
		e = gf_isom_box_array_write(s, ptr->sampleGroups, bs);
		if (e) return e;
	}
	if (ptr->sai_sizes) {
		e = gf_isom_box_array_write(s, ptr->sai_sizes, bs);
		if (e) return e;
	}
	if (ptr->sai_offsets) {
		e = gf_isom_box_array_write(s, ptr->sai_offsets, bs);
		if (e) return e;
	}
	return GF_OK;
}