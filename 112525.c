AP4_DcfStringAtom::AP4_DcfStringAtom(Type            type, 
                                     AP4_UI32        size, 
                                     AP4_UI08        version,
                                     AP4_UI32        flags,
                                     AP4_ByteStream& stream) :
    AP4_Atom(type, size, version, flags)
{
    if (size > AP4_FULL_ATOM_HEADER_SIZE) {
        AP4_UI32 value_size = size-(AP4_FULL_ATOM_HEADER_SIZE);
        char* value = new char[value_size];
        stream.Read(value, value_size);
        m_Value.Assign(value, value_size);
        delete[] value;
    }
}