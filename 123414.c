AP4_HvccAtom::UpdateRawBytes()
{
    AP4_BitWriter bits(23);
    bits.Write(m_ConfigurationVersion, 8);
    bits.Write(m_GeneralProfileSpace, 2);
    bits.Write(m_GeneralTierFlag, 1);
    bits.Write(m_GeneralProfile, 5);
    bits.Write(m_GeneralProfileCompatibilityFlags, 32);
    bits.Write((AP4_UI32)(m_GeneralConstraintIndicatorFlags>>32), 16);
    bits.Write((AP4_UI32)(m_GeneralConstraintIndicatorFlags), 32);
    bits.Write(m_GeneralLevel, 8);
    bits.Write(0xFF, 4);
    bits.Write(m_MinSpatialSegmentation, 12);
    bits.Write(0xFF, 6);
    bits.Write(m_ParallelismType, 2);
    bits.Write(0xFF, 6);
    bits.Write(m_ChromaFormat, 2);
    bits.Write(0xFF, 5);
    bits.Write(m_LumaBitDepth >= 8 ? m_LumaBitDepth - 8 : 0, 3);
    bits.Write(0xFF, 5);
    bits.Write(m_ChromaBitDepth >= 8 ? m_ChromaBitDepth - 8 : 0, 3);
    bits.Write(m_AverageFrameRate, 16);
    bits.Write(m_ConstantFrameRate, 2);
    bits.Write(m_NumTemporalLayers, 3);
    bits.Write(m_TemporalIdNested, 1);
    bits.Write(m_NaluLengthSize > 0 ? m_NaluLengthSize - 1 : 0, 2);
    bits.Write(m_Sequences.ItemCount(), 8);
    
    m_RawBytes.SetData(bits.GetData(), 23);

    for (unsigned int i=0; i<m_Sequences.ItemCount(); i++) {
        AP4_UI08 bytes[3];
        bytes[0] = (m_Sequences[i].m_ArrayCompleteness ? (1<<7) : 0) | m_Sequences[i].m_NaluType;
        AP4_BytesFromUInt16BE(&bytes[1], m_Sequences[i].m_Nalus.ItemCount());
        m_RawBytes.AppendData(bytes, 3);
        
        for (unsigned int j=0; j<m_Sequences[i].m_Nalus.ItemCount(); j++) {
            AP4_UI08 size[2];
            AP4_BytesFromUInt16BE(&size[0], (AP4_UI16)m_Sequences[i].m_Nalus[j].GetDataSize());
            m_RawBytes.AppendData(size, 2);
            m_RawBytes.AppendData(m_Sequences[i].m_Nalus[j].GetData(), m_Sequences[i].m_Nalus[j].GetDataSize());
        }
    }
}