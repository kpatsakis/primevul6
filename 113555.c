AP4_SubtitleSampleEntry::ToSampleDescription()
{
    // create a sample description
    return new AP4_SubtitleSampleDescription(m_Type,
                                             m_Namespace.GetChars(),
                                             m_SchemaLocation.GetChars(),
                                             m_ImageMimeType.GetChars());
}