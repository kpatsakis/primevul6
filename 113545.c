AP4_SampleEntry::AP4_SampleEntry(AP4_Atom::Type format, const AP4_AtomParent* details) :
    AP4_ContainerAtom(format),
    m_DataReferenceIndex(1)
{
    m_Reserved1[0] = 0;
    m_Reserved1[1] = 0;
    m_Reserved1[2] = 0;
    m_Reserved1[3] = 0;
    m_Reserved1[4] = 0;
    m_Reserved1[5] = 0;
    m_Size32 += 8;
    
    if (details) {
        details->CopyChildren(*this);
    }
}