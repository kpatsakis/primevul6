AP4_AvccAtom::UpdateRawBytes()
{
    // compute the payload size
    unsigned int payload_size = 6;    
    for (unsigned int i=0; i<m_SequenceParameters.ItemCount(); i++) {
        payload_size += 2+m_SequenceParameters[i].GetDataSize();
    }
    ++payload_size;
    for (unsigned int i=0; i<m_PictureParameters.ItemCount(); i++) {
        payload_size += 2+m_PictureParameters[i].GetDataSize();
    }
    m_RawBytes.SetDataSize(payload_size);
    AP4_UI08* payload = m_RawBytes.UseData();

    payload[0] = m_ConfigurationVersion;
    payload[1] = m_Profile;
    payload[2] = m_ProfileCompatibility;
    payload[3] = m_Level;
    payload[4] = 0xFC | (m_NaluLengthSize-1);
    payload[5] = 0xE0 | (AP4_UI08)m_SequenceParameters.ItemCount();
    unsigned int cursor = 6;
    for (unsigned int i=0; i<m_SequenceParameters.ItemCount(); i++) {
        AP4_UI16 param_length = (AP4_UI16)m_SequenceParameters[i].GetDataSize();
        AP4_BytesFromUInt16BE(&payload[cursor], param_length);
        cursor += 2;
        AP4_CopyMemory(&payload[cursor], m_SequenceParameters[i].GetData(), param_length);
        cursor += param_length;
    }
    payload[cursor++] = (AP4_UI08)m_PictureParameters.ItemCount();
    for (unsigned int i=0; i<m_PictureParameters.ItemCount(); i++) {
        AP4_UI16 param_length = (AP4_UI16)m_PictureParameters[i].GetDataSize();
        AP4_BytesFromUInt16BE(&payload[cursor], param_length);
        cursor += 2;
        AP4_CopyMemory(&payload[cursor], m_PictureParameters[i].GetData(), param_length);
        cursor += param_length;
    }
}