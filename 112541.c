AP4_MetaData::Entry::RemoveFromFileIlst(AP4_File& file, AP4_Ordinal index)
{
    // look for the 'moov'
    AP4_Movie* movie = file.GetMovie();
    if (movie == NULL) return AP4_ERROR_INVALID_FORMAT;
    AP4_MoovAtom* moov = movie->GetMoovAtom();
    if (moov == NULL) return AP4_ERROR_INVALID_FORMAT;
    
    // look for 'udta/meta/ilst'
    AP4_ContainerAtom* ilst = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moov->FindChild("udta/meta/ilst"));
    if (ilst == NULL) return AP4_ERROR_NO_SUCH_ITEM;
    
    // look if there is already a container for this entry
    AP4_ContainerAtom* existing = FindInIlst(ilst);
    if (existing == NULL) return AP4_ERROR_NO_SUCH_ITEM;
            
    // remove the data atom in the entry
    AP4_Result result = existing->DeleteChild(AP4_ATOM_TYPE_DATA, index);
    if (AP4_FAILED(result)) return result;

    // cleanup
    if (existing->GetType() == AP4_ATOM_TYPE_dddd) {
        // custom entry: if there are no more 'data' children, remove the entry
        if (existing->GetChild(AP4_ATOM_TYPE_DATA) == NULL) {
            ilst->RemoveChild(existing);
            delete existing;
        }
    } else {
        // normal entry: if the entry is empty, remove it
        if (existing->GetChildren().ItemCount() == 0) {
            ilst->RemoveChild(existing);
            delete existing;
        }
    }
    
    return AP4_SUCCESS;
}