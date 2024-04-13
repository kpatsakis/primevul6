md_is_link_destination(MD_CTX* ctx, OFF beg, OFF max_end, OFF* p_end,
                       OFF* p_contents_beg, OFF* p_contents_end)
{
    if(CH(beg) == _T('<'))
        return md_is_link_destination_A(ctx, beg, max_end, p_end, p_contents_beg, p_contents_end);
    else
        return md_is_link_destination_B(ctx, beg, max_end, p_end, p_contents_beg, p_contents_end);
}