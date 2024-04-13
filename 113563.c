AP4_StszAtom::SetSampleSize(AP4_Ordinal sample, AP4_Size sample_size)
{
    // check the sample index
    if (sample > m_SampleCount || sample == 0) {
        return AP4_ERROR_OUT_OF_RANGE;
    } else {
        if (m_Entries.ItemCount() == 0) {
            // all samples must have the same size
            if (sample_size != m_SampleSize) {
                // not the same
                if (sample == 1) {
                    // if this is the first sample, update the global size
                    m_SampleSize = sample_size;
                    return AP4_SUCCESS;
                } else {
                    // can't have different sizes
                    return AP4_ERROR_INVALID_PARAMETERS;
                }
            }
        } else {
            // each sample has a different size
            m_Entries[sample - 1] = sample_size;
        }

        return AP4_SUCCESS;
    }
}