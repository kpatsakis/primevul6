AP4_AtomMetaDataValue::AP4_AtomMetaDataValue(AP4_DataAtom*  atom,
                                             AP4_UI32       parent_type) :
    Value(atom->GetValueType()),
    m_DataAtom(atom)
{
    switch (parent_type) {
        case AP4_ATOM_TYPE_GNRE:
            m_Meaning = MEANING_ID3_GENRE;
            break;

        case AP4_ATOM_TYPE_CPIL:
            m_Meaning = MEANING_BOOLEAN;
            break;

        case AP4_ATOM_TYPE_PGAP:
        case AP4_ATOM_TYPE_PCST:
            m_Meaning = MEANING_BOOLEAN;
            break;

        case AP4_ATOM_TYPE_STIK:
            m_Meaning = MEANING_FILE_KIND;
            break;

        case AP4_ATOM_TYPE_PURL:
        case AP4_ATOM_TYPE_EGID:
            m_Meaning = MEANING_BINARY_ENCODED_CHARS;
            break;
            
        default:
            break;
    }
}