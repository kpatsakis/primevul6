AP4_AtomSampleTable::SetSampleSize(AP4_Ordinal sample_index, AP4_Size size)
{
    if (m_StszAtom) {
        return m_StszAtom->SetSampleSize(sample_index+1, size);
    } else if (m_Stz2Atom) {
        return m_Stz2Atom->SetSampleSize(sample_index+1, size);
    } else {    
        return AP4_FAILURE;
    }
}