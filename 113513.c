AP4_UnknownSampleEntry::AP4_UnknownSampleEntry(AP4_Atom::Type  type, 
                                               AP4_DataBuffer& payload) :
    AP4_SampleEntry(type),
    m_Payload(payload)
{
    m_Size32 += payload.GetDataSize();
}