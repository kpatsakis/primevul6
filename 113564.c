AP4_StszAtom::AP4_StszAtom(AP4_UI32        size, 
                           AP4_UI08        version,
                           AP4_UI32        flags,
                           AP4_ByteStream& stream) :
    AP4_Atom(AP4_ATOM_TYPE_STSZ, size, version, flags)
{
    stream.ReadUI32(m_SampleSize);
    stream.ReadUI32(m_SampleCount);
    if (m_SampleSize == 0) { // means that the samples have different sizes
        // check for overflow
        if (m_SampleCount > (size-8)/4) {
            m_SampleCount = 0;
            return;
        }
        
        // read the entries
        AP4_Cardinal sample_count = m_SampleCount;
        m_Entries.SetItemCount(sample_count);
        unsigned char* buffer = new unsigned char[sample_count*4];
        AP4_Result result = stream.Read(buffer, sample_count*4);
        if (AP4_FAILED(result)) {
            delete[] buffer;
            return;
        }
        for (unsigned int i=0; i<sample_count; i++) {
            m_Entries[i] = AP4_BytesToUInt32BE(&buffer[i*4]);
        }
        delete[] buffer;
    }
}