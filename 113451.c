AP4_AtomSampleTable::GetSampleDescription(AP4_Ordinal index)
{
    return m_StsdAtom ? m_StsdAtom->GetSampleDescription(index) : NULL;
}