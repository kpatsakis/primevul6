    md_get_unicode_fold_info(unsigned codepoint, MD_UNICODE_FOLD_INFO* info)
    {
        info->codepoints[0] = codepoint;
        if(ISUPPER_(codepoint))
            info->codepoints[0] += 'a' - 'A';
        info->n_codepoints = 1;
    }