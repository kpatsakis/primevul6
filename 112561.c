AP4_3GppLocalizedStringAtom::AP4_3GppLocalizedStringAtom(Type            type, 
                                                         AP4_UI32        size, 
                                                         AP4_UI08        version,
                                                         AP4_UI32        flags,
                                                         AP4_ByteStream& stream) :
    AP4_Atom(type, size, version, flags)
{
    // read the language code
    AP4_UI16 packed_language;
    stream.ReadUI16(packed_language);
    m_Language[0] = 0x60+((packed_language>>10)&0x1F);
    m_Language[1] = 0x60+((packed_language>> 5)&0x1F);
    m_Language[2] = 0x60+((packed_language    )&0x1F);
    m_Language[3] = '\0';
    
    // read the value (should be a NULL-terminated string, but we'll
    // allow for strings that are not terminated)
    if (size > AP4_FULL_ATOM_HEADER_SIZE+2) {
        AP4_UI32 value_size = size-(AP4_FULL_ATOM_HEADER_SIZE+2);
        char* value = new char[value_size];
        stream.Read(value, value_size);
        m_Value.Assign(value, value_size);
        delete[] value;
    }
}