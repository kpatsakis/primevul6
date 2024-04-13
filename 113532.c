AP4_MpegAudioSampleEntry::ToSampleDescription()
{
    // find the esds atom
    AP4_EsdsAtom* esds = AP4_DYNAMIC_CAST(AP4_EsdsAtom, GetChild(AP4_ATOM_TYPE_ESDS));
    if (esds == NULL) {
        // check if this is a quicktime style sample description
        if (m_QtVersion > 0) {
            esds = AP4_DYNAMIC_CAST(AP4_EsdsAtom, FindChild("wave/esds"));
        }
    }
    
    // create a sample description
    return new AP4_MpegAudioSampleDescription(GetSampleRate(),
                                              GetSampleSize(),
                                              GetChannelCount(),
                                              esds);
}