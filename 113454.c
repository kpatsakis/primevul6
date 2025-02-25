AP4_AtomSampleTable::GetSample(AP4_Ordinal index, 
                               AP4_Sample& sample)
{
    AP4_Result result;

    // check that we have an stsc atom
    if (!m_StscAtom) {
        return AP4_ERROR_INVALID_FORMAT;
    }
    
    // check that we have a chunk offset table
    if (m_StcoAtom == NULL && m_Co64Atom == NULL) {
        return AP4_ERROR_INVALID_FORMAT;
    }

    // MP4 uses 1-based indexes internally, so adjust by one
    index++;

    // find out in which chunk this sample is located
    AP4_Ordinal chunk, skip, desc;
    result = m_StscAtom->GetChunkForSample(index, chunk, skip, desc);
    if (AP4_FAILED(result)) return result;
    
    // check that the result is within bounds
    if (skip > index) return AP4_ERROR_INTERNAL;

    // get the atom offset for this chunk
    AP4_UI64 offset;
    if (m_StcoAtom) {
        AP4_UI32 offset_32;
        result = m_StcoAtom->GetChunkOffset(chunk, offset_32);
        offset = offset_32;
    } else {
        result = m_Co64Atom->GetChunkOffset(chunk, offset);
    }
    if (AP4_FAILED(result)) return result;
    
    // compute the additional offset inside the chunk
    for (unsigned int i = index-skip; i < index; i++) {
        AP4_Size size = 0;
        if (m_StszAtom) {
            result = m_StszAtom->GetSampleSize(i, size); 
        } else if (m_Stz2Atom) {
            result = m_Stz2Atom->GetSampleSize(i, size); 
        } else {
            result = AP4_ERROR_INVALID_FORMAT;
        }
        if (AP4_FAILED(result)) return result;
        offset += size;
    }

    // set the description index
    sample.SetDescriptionIndex(desc-1); // adjust for 0-based indexes

    // set the dts and cts
    AP4_UI32 cts_offset = 0;
    AP4_UI64 dts        = 0;
    AP4_UI32 duration   = 0;
    if (m_SttsAtom) {
        result = m_SttsAtom->GetDts(index, dts, &duration);
        if (AP4_FAILED(result)) return result;
    }
    sample.SetDuration(duration);
    sample.SetDts(dts);
    if (m_CttsAtom == NULL) {
        sample.SetCts(dts);
    } else {
        result = m_CttsAtom->GetCtsOffset(index, cts_offset); 
	    if (AP4_FAILED(result)) return result;
        sample.SetCtsDelta(cts_offset);
    }     

    // set the size
    AP4_Size sample_size = 0;
    if (m_StszAtom) {
        result = m_StszAtom->GetSampleSize(index, sample_size); 
    } else if (m_Stz2Atom) {
        result = m_Stz2Atom->GetSampleSize(index, sample_size); 
    } else {
        result = AP4_ERROR_INVALID_FORMAT;
    }
    if (AP4_FAILED(result)) return result;
    sample.SetSize(sample_size);

    // set the sync flag
    if (m_StssAtom == NULL) {
        sample.SetSync(true);
    } else {
        sample.SetSync(m_StssAtom->IsSampleSync(index));
    }

    // set the offset
    sample.SetOffset(offset);

    // set the data stream
    sample.SetDataStream(m_SampleStream);


    return AP4_SUCCESS;
}