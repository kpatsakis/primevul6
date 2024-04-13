md_ref_def_cmp_for_sort(const void* a, const void* b)
{
    int cmp;

    cmp = md_ref_def_cmp(a, b);

    /* Ensure stability of the sorting. */
    if(cmp == 0) {
        const MD_REF_DEF* a_ref = *(const MD_REF_DEF**)a;
        const MD_REF_DEF* b_ref = *(const MD_REF_DEF**)b;

        if(a_ref < b_ref)
            cmp = -1;
        else if(a_ref > b_ref)
            cmp = +1;
        else
            cmp = 0;
    }

    return cmp;
}