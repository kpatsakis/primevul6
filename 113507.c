AP4_VisualSampleEntry::AP4_VisualSampleEntry(
    AP4_Atom::Type    format, 
    AP4_UI16          width,
    AP4_UI16          height,
    AP4_UI16          depth,
    const char*           compressor_name,
    const AP4_AtomParent* details) :
    AP4_SampleEntry(format, details),
    m_Predefined1(0),
    m_Reserved2(0),
    m_Width(width),
    m_Height(height),
    m_HorizResolution(0x00480000),
    m_VertResolution(0x00480000),
    m_Reserved3(0),
    m_FrameCount(1),
    m_CompressorName(compressor_name),
    m_Depth(depth),
    m_Predefined3(0xFFFF)
{
    memset(m_Predefined2, 0, sizeof(m_Predefined2));
    m_Size32 += 70;
}