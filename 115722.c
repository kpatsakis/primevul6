FindFragmentMapEntry(AP4_Array<FragmentMapEntry>& fragment_map, AP4_UI64 fragment_offset) {
    int first = 0;
    int last = fragment_map.ItemCount();
    while (first < last) {
        int middle = (last+first)/2;
        AP4_UI64 middle_value = fragment_map[middle].before;
        if (fragment_offset < middle_value) {
            last = middle;
        } else if (fragment_offset > middle_value) {
            first = middle+1;
        } else {
            return &fragment_map[middle];
        }
    }
    
    return NULL;
}