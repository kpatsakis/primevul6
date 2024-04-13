AP4_AtomSampleTable::GetSampleChunkPosition(AP4_Ordinal  sample_index, 
                                            AP4_Ordinal& chunk_index,
                                            AP4_Ordinal& position_in_chunk)
{
    // default values
    chunk_index       = 0;
    position_in_chunk = 0;
    
    AP4_Ordinal sample_description_index;
    return GetChunkForSample(sample_index, 
                             chunk_index, 
                             position_in_chunk, 
                             sample_description_index);
}