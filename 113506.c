AP4_UnknownSampleEntry::AP4_UnknownSampleEntry(AP4_Atom::Type  type, 
                                               AP4_Size        size, 
                                               AP4_ByteStream& stream) :
    AP4_SampleEntry(type, size)
{
    if (size > AP4_ATOM_HEADER_SIZE+AP4_SampleEntry::GetFieldsSize()) {
        m_Payload.SetDataSize(size-(AP4_ATOM_HEADER_SIZE+AP4_SampleEntry::GetFieldsSize()));
        ReadFields(stream);
    }
}