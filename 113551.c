AP4_MpegVideoSampleEntry::ToSampleDescription()
{
    // create a sample description
    return new AP4_MpegVideoSampleDescription(
        m_Width,
        m_Height,
        m_Depth,
        m_CompressorName.GetChars(),
        AP4_DYNAMIC_CAST(AP4_EsdsAtom, GetChild(AP4_ATOM_TYPE_ESDS)));
}