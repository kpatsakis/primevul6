AP4_AtomSampleTable::SetChunkOffset(AP4_Ordinal  chunk_index, 
                                    AP4_Position offset)
{
    if (m_StcoAtom) {
        if ((offset >> 32) != 0) return AP4_ERROR_OUT_OF_RANGE;
        return m_StcoAtom->SetChunkOffset(chunk_index+1, (AP4_UI32)offset);
    } else if (m_Co64Atom) {
        return m_Co64Atom->SetChunkOffset(chunk_index+1, offset);
    } else {
        return AP4_FAILURE;
    }
}