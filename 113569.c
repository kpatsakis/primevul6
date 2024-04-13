AP4_StszAtom::AddEntry(AP4_UI32 size)
{
    m_Entries.Append(size);
    m_SampleCount++;
    m_Size32 += 4;

    return AP4_SUCCESS;
}