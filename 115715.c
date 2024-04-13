AP4_AvccAtom::AP4_AvccAtom(AP4_UI08                         profile, 
                           AP4_UI08                         level, 
                           AP4_UI08                         profile_compatibility, 
                           AP4_UI08                         length_size, 
                           const AP4_Array<AP4_DataBuffer>& sequence_parameters, 
                           const AP4_Array<AP4_DataBuffer>& picture_parameters) :
    AP4_Atom(AP4_ATOM_TYPE_AVCC, AP4_ATOM_HEADER_SIZE),
    m_ConfigurationVersion(1),
    m_Profile(profile),
    m_Level(level),
    m_ProfileCompatibility(profile_compatibility),
    m_NaluLengthSize(length_size)
{
    // deep copy of the parameters
    unsigned int i = 0;
    for (i=0; i<sequence_parameters.ItemCount(); i++) {
        m_SequenceParameters.Append(sequence_parameters[i]);
    }
    for (i=0; i<picture_parameters.ItemCount(); i++) {
        m_PictureParameters.Append(picture_parameters[i]);
    }    

    // compute the raw bytes
    UpdateRawBytes();

    // update the size
    m_Size32 += m_RawBytes.GetDataSize();
}