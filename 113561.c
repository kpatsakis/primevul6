AP4_StszAtom::GetSampleSize(AP4_Ordinal sample, AP4_Size& sample_size)
{
    // check the sample index
    if (sample > m_SampleCount || sample == 0) {
        sample_size = 0;
        return AP4_ERROR_OUT_OF_RANGE;
    } else {
        // find the size
        if (m_SampleSize != 0) { // constant size
            sample_size = m_SampleSize;
        } else {
            sample_size = m_Entries[sample - 1];
        }
        return AP4_SUCCESS;
    }
}