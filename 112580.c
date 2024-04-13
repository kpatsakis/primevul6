AP4_MetaData::ResolveKeyName(AP4_Atom::Type atom_type, AP4_String& value)
{
    const char* key_name = NULL;
    char        four_cc[5];

    // look for a match in the key infos
    for (unsigned int i=0; 
         i<sizeof(AP4_MetaData_KeyInfos)/sizeof(AP4_MetaData_KeyInfos[0]); 
         i++) {
        if (AP4_MetaData_KeyInfos[i].four_cc == atom_type) {
            key_name = AP4_MetaData_KeyInfos[i].name;
            break;
        }
    }
    if (key_name == NULL) {
        // this key was not found in the key infos, create a name for it
        AP4_FormatFourChars(four_cc, (AP4_UI32)atom_type);
        key_name = four_cc;
    }
    value = key_name;
    
    return AP4_SUCCESS;
}