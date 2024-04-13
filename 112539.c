AP4_MetaData::Add3GppEntry(AP4_3GppLocalizedStringAtom* atom, const char* namespc)
{
    AP4_String key_name;
    ResolveKeyName(atom->GetType(), key_name);
    
    const char* language = NULL;
    if (atom->GetLanguage()[0]) {
        language = atom->GetLanguage();
    }
    AP4_MetaData::Value* value = new AP4_StringMetaDataValue(atom->GetValue().GetChars(),
                                                             language);
    m_Entries.Add(new Entry(key_name.GetChars(), namespc, value));
    
    return AP4_SUCCESS;
}