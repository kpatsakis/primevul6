may_start_select(int c)
{
    VIsual_select = (c == 'o' || (stuff_empty() && typebuf_typed()))
		    && vim_strchr(p_slm, c) != NULL;
}