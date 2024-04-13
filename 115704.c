AP4_Processor::CreateFragmentHandler(AP4_TrakAtom*      /* trak */,
                                     AP4_TrexAtom*      /* trex */,
                                     AP4_ContainerAtom* traf,
                                     AP4_ByteStream&    /* moof_data   */,
                                     AP4_Position       /* moof_offset */)
{
    // find the matching track handler
    for (unsigned int i=0; i<m_TrackIds.ItemCount(); i++) {
        AP4_TfhdAtom* tfhd = AP4_DYNAMIC_CAST(AP4_TfhdAtom, traf->GetChild(AP4_ATOM_TYPE_TFHD));
        if (tfhd && m_TrackIds[i] == tfhd->GetTrackId()) {
            return new AP4_DefaultFragmentHandler(m_TrackHandlers[i]);
        }
    }
    
    return NULL;
}