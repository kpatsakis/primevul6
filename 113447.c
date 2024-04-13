AP4_AtomSampleTable::AP4_AtomSampleTable(AP4_ContainerAtom* stbl, 
                                         AP4_ByteStream&    sample_stream) :
    m_SampleStream(sample_stream)
{
    m_StscAtom = AP4_DYNAMIC_CAST(AP4_StscAtom, stbl->GetChild(AP4_ATOM_TYPE_STSC));
    m_StcoAtom = AP4_DYNAMIC_CAST(AP4_StcoAtom, stbl->GetChild(AP4_ATOM_TYPE_STCO));
    m_StszAtom = AP4_DYNAMIC_CAST(AP4_StszAtom, stbl->GetChild(AP4_ATOM_TYPE_STSZ));
    m_Stz2Atom = AP4_DYNAMIC_CAST(AP4_Stz2Atom, stbl->GetChild(AP4_ATOM_TYPE_STZ2));
    m_CttsAtom = AP4_DYNAMIC_CAST(AP4_CttsAtom, stbl->GetChild(AP4_ATOM_TYPE_CTTS));
    m_SttsAtom = AP4_DYNAMIC_CAST(AP4_SttsAtom, stbl->GetChild(AP4_ATOM_TYPE_STTS));
    m_StssAtom = AP4_DYNAMIC_CAST(AP4_StssAtom, stbl->GetChild(AP4_ATOM_TYPE_STSS));
    m_StsdAtom = AP4_DYNAMIC_CAST(AP4_StsdAtom, stbl->GetChild(AP4_ATOM_TYPE_STSD));
    m_Co64Atom = AP4_DYNAMIC_CAST(AP4_Co64Atom, stbl->GetChild(AP4_ATOM_TYPE_CO64));

    // keep a reference to the sample stream
    m_SampleStream.AddReference();
}