AP4_AtomSampleTable::GetChunkForSample(AP4_Ordinal  sample_index,
                                       AP4_Ordinal& chunk_index,
                                       AP4_Ordinal& position_in_chunk,
                                       AP4_Ordinal& sample_description_index)
{
    // default values
    chunk_index              = 0;
    position_in_chunk        = 0;
    sample_description_index = 0;

    // check that we an stsc atom
    if (m_StscAtom == NULL) return AP4_ERROR_INVALID_STATE;
    
    // get the chunk info from the stsc atom
    AP4_Ordinal chunk = 0;
    AP4_Result result = m_StscAtom->GetChunkForSample(sample_index+1, // the atom API is 1-based 
                                                      chunk, 
                                                      position_in_chunk, 
                                                      sample_description_index);
    if (AP4_FAILED(result)) return result;
    if (chunk == 0) return AP4_ERROR_INTERNAL;

    // the atom sample and chunk indexes are 1-based, so we need to translate
    chunk_index = chunk-1;
    
    return AP4_SUCCESS;
}