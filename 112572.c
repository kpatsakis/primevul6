AP4_3GppLocalizedStringAtom::WriteFields(AP4_ByteStream& stream)
{
    AP4_UI16 packed_language = ((m_Language[0]-0x60)<<10) |
                               ((m_Language[1]-0x60)<< 5) |
                               ((m_Language[2]-0x60));
    stream.WriteUI16(packed_language);
    AP4_Size payload_size = (AP4_UI32)GetSize()-GetHeaderSize();
    if (payload_size < 2) return AP4_ERROR_INVALID_FORMAT;
    AP4_Size value_size = m_Value.GetLength()+1;
    if (value_size > payload_size-2) {
        value_size = payload_size-2;
    }
    stream.Write(m_Value.GetChars(), value_size);
    for (unsigned int i=value_size; i<payload_size-2; i++) {
        stream.WriteUI08(0);
    }
    return AP4_SUCCESS;
}