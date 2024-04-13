AP4_StszAtom::AP4_StszAtom() :
    AP4_Atom(AP4_ATOM_TYPE_STSZ, AP4_FULL_ATOM_HEADER_SIZE+8, 0, 0),
    m_SampleSize(0),
    m_SampleCount(0)
{
}