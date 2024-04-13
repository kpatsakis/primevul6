md_ref_def_cmp(const void* a, const void* b)
{
    const MD_REF_DEF* a_ref = *(const MD_REF_DEF**)a;
    const MD_REF_DEF* b_ref = *(const MD_REF_DEF**)b;

    if(a_ref->hash < b_ref->hash)
        return -1;
    else if(a_ref->hash > b_ref->hash)
        return +1;
    else
        return md_link_label_cmp(a_ref->label, a_ref->label_size, b_ref->label, b_ref->label_size);
}