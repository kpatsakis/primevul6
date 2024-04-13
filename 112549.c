AP4_MetaData::Entry::ToAtom(AP4_Atom*& atom) const
{
    atom = NULL;
    
    if (m_Value == NULL) {
        return AP4_ERROR_INVALID_PARAMETERS;
    }
    
    if (m_Key.GetNamespace() == "meta") {
        // convert the name into an atom type
        if (m_Key.GetName().GetLength() != 4) {
            // the name is not in the right format
            return AP4_ERROR_INVALID_PARAMETERS;
        }
        AP4_Atom::Type atom_type = AP4_Atom::TypeFromString(m_Key.GetName().GetChars()); 
                                   
        // create a container atom for the data
        AP4_ContainerAtom* container = new AP4_ContainerAtom(atom_type);

        // add the data atom
        AP4_DataAtom* data = new AP4_DataAtom(*m_Value);
        container->AddChild(data);
        
        atom = container;
        return AP4_SUCCESS;
    } else if (m_Key.GetNamespace() == "dcf") {
        // convert the name into an atom type
        if (m_Key.GetName().GetLength() != 4) {
            // the name is not in the right format
            return AP4_ERROR_INVALID_PARAMETERS;
        }
        AP4_Atom::Type atom_type = AP4_Atom::TypeFromString(m_Key.GetName().GetChars()); 

        if (AP4_MetaDataAtomTypeHandler::IsTypeInList(atom_type, 
            AP4_MetaDataAtomTypeHandler::DcfStringTypeList)) {
            AP4_String atom_value = m_Value->ToString();
            atom = new AP4_DcfStringAtom(atom_type, atom_value.GetChars());
            return AP4_SUCCESS;
        } else if (AP4_MetaDataAtomTypeHandler::IsTypeInList(atom_type, 
                   AP4_MetaDataAtomTypeHandler::_3gppLocalizedStringTypeList)) {
            AP4_String atom_value = m_Value->ToString();
            const char* language = "eng"; // default
            if (m_Value->GetLanguage().GetLength() != 0) {
                language = m_Value->GetLanguage().GetChars();
            }
            atom = new AP4_3GppLocalizedStringAtom(atom_type, language, atom_value.GetChars());
            return AP4_SUCCESS;
        } else if (atom_type == AP4_ATOM_TYPE_DCFD) {
            atom = new AP4_DcfdAtom((AP4_UI32)m_Value->ToInteger());
            return AP4_SUCCESS;
        }
        
         // not supported
         return AP4_ERROR_NOT_SUPPORTED; 
    } else {
        // create a '----' atom
        AP4_ContainerAtom* container = new AP4_ContainerAtom(AP4_ATOM_TYPE_dddd);
        
        // add a 'mean' string
        container->AddChild(new AP4_MetaDataStringAtom(AP4_ATOM_TYPE_MEAN, m_Key.GetNamespace().GetChars()));
        
        // add a 'name' string
        container->AddChild(new AP4_MetaDataStringAtom(AP4_ATOM_TYPE_NAME, m_Key.GetName().GetChars()));

        // add the data atom
        AP4_DataAtom* data = new AP4_DataAtom(*m_Value);
        container->AddChild(data);
        
        atom = container;
        return AP4_SUCCESS;
    }
        
    // unreachable - return AP4_ERROR_NOT_SUPPORTED;
}