AP4_AvcSampleEntry::ToSampleDescription()
{
    return new AP4_AvcSampleDescription(
        m_Type,
        m_Width,
        m_Height,
        m_Depth,
        m_CompressorName.GetChars(),
        this);
}