AP4_MetaData::AddIlstEntries(AP4_ContainerAtom* atom, const char* namespc)
{
    AP4_MetaData::Value* value = NULL;

    if (atom->GetType() == AP4_ATOM_TYPE_dddd) {
        // look for the namespace
        AP4_MetaDataStringAtom* mean = static_cast<AP4_MetaDataStringAtom*>(atom->GetChild(AP4_ATOM_TYPE_MEAN));
        if (mean == NULL) return AP4_ERROR_INVALID_FORMAT;

        // look for the name
        AP4_MetaDataStringAtom* name = static_cast<AP4_MetaDataStringAtom*>(atom->GetChild(AP4_ATOM_TYPE_NAME));
        if (name == NULL) return AP4_ERROR_INVALID_FORMAT;

        // get the value
        AP4_DataAtom* data_atom = static_cast<AP4_DataAtom*>(atom->GetChild(AP4_ATOM_TYPE_DATA));
        if (data_atom == NULL) return AP4_ERROR_INVALID_FORMAT;
        value = new AP4_AtomMetaDataValue(data_atom, atom->GetType());
        
        return m_Entries.Add(new Entry(name->GetValue().GetChars(),
                                       mean->GetValue().GetChars(),
                                       value));
    } else {
        const char* key_name = NULL;
        char        four_cc[5];

        // convert the atom type to a name
        AP4_FormatFourChars(four_cc, (AP4_UI32)atom->GetType());
        key_name = four_cc;

        // add one entry for each data atom
        AP4_List<AP4_Atom>::Item* data_item = atom->GetChildren().FirstItem();
        while (data_item) {
            AP4_Atom* item_atom = data_item->GetData();
            if (item_atom->GetType() == AP4_ATOM_TYPE_DATA) {
                AP4_DataAtom* data_atom = static_cast<AP4_DataAtom*>(item_atom);
                value = new AP4_AtomMetaDataValue(data_atom, atom->GetType());
                m_Entries.Add(new Entry(key_name, namespc, value));
            }
            data_item = data_item->GetNext();
        }

        return AP4_SUCCESS;
    }
}