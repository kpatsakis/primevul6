AP4_UnknownSampleEntry::GetFieldsSize()
{
    return AP4_SampleEntry::GetFieldsSize()+m_Payload.GetDataSize();
}