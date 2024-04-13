AP4_VisualSampleEntry::ToSampleDescription()
{
    // create a sample description
    return new AP4_GenericVideoSampleDescription(
        m_Type,
        m_Width,
        m_Height,
        m_Depth,
        m_CompressorName.GetChars(),
        this);
}