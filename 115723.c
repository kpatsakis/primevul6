AP4_AvccAtom::AP4_AvccAtom(const AP4_AvccAtom& other) :
    AP4_Atom(AP4_ATOM_TYPE_AVCC, other.m_Size32),
    m_ConfigurationVersion(other.m_ConfigurationVersion),
    m_Profile(other.m_Profile),
    m_Level(other.m_Level),
    m_ProfileCompatibility(other.m_ProfileCompatibility),
    m_NaluLengthSize(other.m_NaluLengthSize),
    m_RawBytes(other.m_RawBytes)
{
    // deep copy of the parameters
    unsigned int i = 0;
    for (i=0; i<other.m_SequenceParameters.ItemCount(); i++) {
        m_SequenceParameters.Append(other.m_SequenceParameters[i]);
    }
    for (i=0; i<other.m_PictureParameters.ItemCount(); i++) {
        m_PictureParameters.Append(other.m_PictureParameters[i]);
    }    
}