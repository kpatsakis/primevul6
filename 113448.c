AP4_AtomSampleTable::GetChunkOffset(AP4_Ordinal   chunk_index, 
                                    AP4_Position& offset)
{
    if (m_StcoAtom) {
        AP4_UI32 offset_32;
        AP4_Result result = m_StcoAtom->GetChunkOffset(chunk_index+1, offset_32);
        if (AP4_SUCCEEDED(result)) {
            offset = offset_32;
        } else {
            offset = 0;
        }
        return result;
    } else if (m_Co64Atom) {
        return m_Co64Atom->GetChunkOffset(chunk_index+1, offset);
    } else {
        offset = 0;
        return AP4_FAILURE;
    }
}