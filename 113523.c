AP4_SampleEntry::AP4_SampleEntry(AP4_Atom::Type format,
                                 AP4_Size       size) :
    AP4_ContainerAtom(format, (AP4_UI64)size, false),
    m_DataReferenceIndex(1)
{
    m_Reserved1[0] = 0;
    m_Reserved1[1] = 0;
    m_Reserved1[2] = 0;
    m_Reserved1[3] = 0;
    m_Reserved1[4] = 0;
    m_Reserved1[5] = 0;
}