GF_Err gf_isom_get_fragment_defaults(GF_ISOFile *the_file, u32 trackNumber,
                                     u32 *defaultDuration, u32 *defaultSize, u32 *defaultDescriptionIndex,
                                     u32 *defaultRandomAccess, u8 *defaultPadding, u16 *defaultDegradationPriority)
{
	GF_TrackBox *trak;
	GF_StscEntry *sc_ent;
	u32 i, j, maxValue, value;
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	GF_TrackExtendsBox *trex;
#endif
	GF_SampleTableBox *stbl;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	/*if trex is already set, restore flags*/
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	trex = the_file->moov->mvex ? GetTrex(the_file->moov, gf_isom_get_track_id(the_file,trackNumber) ) : NULL;
	if (trex) {
		trex->track = trak;

		if (defaultDuration) *defaultDuration = trex->def_sample_duration;
		if (defaultSize) *defaultSize = trex->def_sample_size;
		if (defaultDescriptionIndex) *defaultDescriptionIndex = trex->def_sample_desc_index;
		if (defaultRandomAccess) *defaultRandomAccess = GF_ISOM_GET_FRAG_SYNC(trex->def_sample_flags);
		if (defaultPadding) *defaultPadding = GF_ISOM_GET_FRAG_PAD(trex->def_sample_flags);
		if (defaultDegradationPriority) *defaultDegradationPriority = GF_ISOM_GET_FRAG_DEG(trex->def_sample_flags);
		return GF_OK;
	}
#endif

	stbl = trak->Media->information->sampleTable;
	if (!stbl->TimeToSample || !stbl->SampleSize || !stbl->SampleToChunk) return GF_ISOM_INVALID_FILE;


	//duration
	if (defaultDuration) {
		maxValue = value = 0;
		for (i=0; i<stbl->TimeToSample->nb_entries; i++) {
			if (stbl->TimeToSample->entries[i].sampleCount>maxValue) {
				value = stbl->TimeToSample->entries[i].sampleDelta;
				maxValue = stbl->TimeToSample->entries[i].sampleCount;
			}
		}
		*defaultDuration = value;
	}
	//size
	if (defaultSize) {
		*defaultSize = stbl->SampleSize->sampleSize;
	}
	//descIndex
	if (defaultDescriptionIndex) {
		GF_SampleToChunkBox *stsc= stbl->SampleToChunk;
		maxValue = value = 0;
		for (i=0; i<stsc->nb_entries; i++) {
			sc_ent = &stsc->entries[i];
			if ((sc_ent->nextChunk - sc_ent->firstChunk) * sc_ent->samplesPerChunk > maxValue) {
				value = sc_ent->sampleDescriptionIndex;
				maxValue = (sc_ent->nextChunk - sc_ent->firstChunk) * sc_ent->samplesPerChunk;
			}
		}
		*defaultDescriptionIndex = value ? value : 1;
	}
	//RAP
	if (defaultRandomAccess) {
		//no sync table is ALL RAP
		*defaultRandomAccess = stbl->SyncSample ? 0 : 1;
		if (stbl->SyncSample
		        && (stbl->SyncSample->nb_entries == stbl->SampleSize->sampleCount)) {
			*defaultRandomAccess = 1;
		}
	}
	//defaultPadding
	if (defaultPadding) {
		*defaultPadding = 0;
		if (stbl->PaddingBits) {
			maxValue = 0;
			for (i=0; i<stbl->PaddingBits->SampleCount; i++) {
				value = 0;
				for (j=0; j<stbl->PaddingBits->SampleCount; j++) {
					if (stbl->PaddingBits->padbits[i]==stbl->PaddingBits->padbits[j]) {
						value ++;
					}
				}
				if (value>maxValue) {
					maxValue = value;
					*defaultPadding = stbl->PaddingBits->padbits[i];
				}
			}
		}
	}
	//defaultDegradationPriority
	if (defaultDegradationPriority) {
		*defaultDegradationPriority = 0;
		if (stbl->DegradationPriority) {
			maxValue = 0;
			for (i=0; i<stbl->DegradationPriority->nb_entries; i++) {
				value = 0;
				for (j=0; j<stbl->DegradationPriority->nb_entries; j++) {
					if (stbl->DegradationPriority->priorities[i]==stbl->DegradationPriority->priorities[j]) {
						value ++;
					}
				}
				if (value>maxValue) {
					maxValue = value;
					*defaultDegradationPriority = stbl->DegradationPriority->priorities[i];
				}
			}
		}
	}
	return GF_OK;
}