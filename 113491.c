AP4_HevcSampleEntry::ToSampleDescription()
{
    return new AP4_HevcSampleDescription(
        m_Type,
        m_Width,
        m_Height,
        m_Depth,
        m_CompressorName.GetChars(),
        this);
}