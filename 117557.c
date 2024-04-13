md_asterisk_chain(MD_CTX* ctx, unsigned flags)
{
    switch(flags & (MD_MARK_EMPH_INTRAWORD | MD_MARK_EMPH_MOD3_MASK)) {
        case MD_MARK_EMPH_INTRAWORD | MD_MARK_EMPH_MOD3_0:  return &ASTERISK_OPENERS_intraword_mod3_0;
        case MD_MARK_EMPH_INTRAWORD | MD_MARK_EMPH_MOD3_1:  return &ASTERISK_OPENERS_intraword_mod3_1;
        case MD_MARK_EMPH_INTRAWORD | MD_MARK_EMPH_MOD3_2:  return &ASTERISK_OPENERS_intraword_mod3_2;
        case MD_MARK_EMPH_MOD3_0:                           return &ASTERISK_OPENERS_extraword_mod3_0;
        case MD_MARK_EMPH_MOD3_1:                           return &ASTERISK_OPENERS_extraword_mod3_1;
        case MD_MARK_EMPH_MOD3_2:                           return &ASTERISK_OPENERS_extraword_mod3_2;
        default:                                            MD_UNREACHABLE();
    }
    return NULL;
}