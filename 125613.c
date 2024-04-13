void streamIteratorGetField(streamIterator *si, unsigned char **fieldptr, unsigned char **valueptr, int64_t *fieldlen, int64_t *valuelen) {
    if (si->entry_flags & STREAM_ITEM_FLAG_SAMEFIELDS) {
        *fieldptr = lpGet(si->master_fields_ptr,fieldlen,si->field_buf);
        si->master_fields_ptr = lpNext(si->lp,si->master_fields_ptr);
    } else {
        *fieldptr = lpGet(si->lp_ele,fieldlen,si->field_buf);
        si->lp_ele = lpNext(si->lp,si->lp_ele);
    }
    *valueptr = lpGet(si->lp_ele,valuelen,si->value_buf);
    si->lp_ele = lpNext(si->lp,si->lp_ele);
}