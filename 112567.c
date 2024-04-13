AP4_MetaData::Initialize() {
    unsigned int item_count = sizeof(AP4_MetaData_KeyInfos)/sizeof(KeyInfo);
    KeyInfos.SetItemCount(item_count);
    for (unsigned int i=0; i<item_count; i++) {
        KeyInfos[i] = AP4_MetaData_KeyInfos[i];
    }
    return AP4_SUCCESS;
}