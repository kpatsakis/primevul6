AP4_MetaData::Entry::FindInIlst(AP4_ContainerAtom* ilst) const
{
    if (m_Key.GetNamespace() == "meta") {
        AP4_Atom::Type atom_type = AP4_Atom::TypeFromString(m_Key.GetName().GetChars());
        return AP4_DYNAMIC_CAST(AP4_ContainerAtom, ilst->GetChild(atom_type));
    } else {
        AP4_List<AP4_Atom>::Item* ilst_item = ilst->GetChildren().FirstItem();
        while (ilst_item) {
            AP4_ContainerAtom* entry_atom = AP4_DYNAMIC_CAST(AP4_ContainerAtom, ilst_item->GetData()); 
            if (entry_atom) {
                AP4_MetaDataStringAtom* mean = static_cast<AP4_MetaDataStringAtom*>(entry_atom->GetChild(AP4_ATOM_TYPE_MEAN));
                AP4_MetaDataStringAtom* name = static_cast<AP4_MetaDataStringAtom*>(entry_atom->GetChild(AP4_ATOM_TYPE_NAME));
                if (mean && name &&
                    mean->GetValue() == m_Key.GetNamespace() &&
                    name->GetValue() == m_Key.GetName()) {
                    return entry_atom;
                }
            }
            ilst_item = ilst_item->GetNext();
        }
    }
    
    // not found
    return NULL;
}