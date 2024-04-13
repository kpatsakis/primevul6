md_analyze_table_alignment(MD_CTX* ctx, OFF beg, OFF end, MD_ALIGN* align, int n_align)
{
    static const MD_ALIGN align_map[] = { MD_ALIGN_DEFAULT, MD_ALIGN_LEFT, MD_ALIGN_RIGHT, MD_ALIGN_CENTER };
    OFF off = beg;

    while(n_align > 0) {
        int index = 0;  /* index into align_map[] */

        while(CH(off) != _T('-'))
            off++;
        if(off > beg  &&  CH(off-1) == _T(':'))
            index |= 1;
        while(off < end  &&  CH(off) == _T('-'))
            off++;
        if(off < end  &&  CH(off) == _T(':'))
            index |= 2;

        *align = align_map[index];
        align++;
        n_align--;
    }

}