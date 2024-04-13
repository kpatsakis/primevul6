AP4_SubtitleSampleEntry::GetFieldsSize()
{
    return AP4_SampleEntry::GetFieldsSize() +
        3                                   +
        m_Namespace.GetLength()             +
        m_SchemaLocation.GetLength()        +
        m_ImageMimeType.GetLength();
}