AP4_AvccAtom::AP4_AvccAtom() :
    AP4_Atom(AP4_ATOM_TYPE_AVCC, AP4_ATOM_HEADER_SIZE),
    m_ConfigurationVersion(1),
    m_Profile(0),
    m_Level(0),
    m_ProfileCompatibility(0),
    m_NaluLengthSize(0)
{
    UpdateRawBytes();
    m_Size32 += m_RawBytes.GetDataSize();
}