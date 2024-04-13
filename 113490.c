AP4_AudioSampleEntry::GetFieldsSize()
{
    AP4_Size size = AP4_SampleEntry::GetFieldsSize()+20;
    if (m_QtVersion == 1) {
        size += 16;
    } else if (m_QtVersion == 2) {
        size += 36+m_QtV2Extension.GetDataSize();
    }
    
    return size;
}