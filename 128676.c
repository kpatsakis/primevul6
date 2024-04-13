lookup_gs_simple_font_encoding(gs_font_base * pfont)
{
    const ref *pfe = &pfont_data(pfont)->Encoding;
    int index = -1;

    pfont->encoding_index = index;
    if (r_type(pfe) == t_array && r_size(pfe) <= 256) {
        /* Look for an encoding that's "close". */
        uint esize = r_size(pfe);
        int near_index = -1;
        uint best = esize / 3;	/* must match at least this many */
        gs_const_string fstrs[256];
        int i;

        /* Get the string names of the glyphs in the font's Encoding. */
        for (i = 0; i < esize; ++i) {
            ref fchar;

            if (array_get(pfont->memory, pfe, (long)i, &fchar) < 0 ||
                !r_has_type(&fchar, t_name)
                )
                fstrs[i].data = 0, fstrs[i].size = 0;
            else {
                ref nsref;

                name_string_ref(pfont->memory, &fchar, &nsref);
                fstrs[i].data = nsref.value.const_bytes;
                fstrs[i].size = r_size(&nsref);
            }
        }
        /* Compare them against the known encodings. */
        for (index = 0; index < NUM_KNOWN_REAL_ENCODINGS; ++index) {
            uint match = esize;

            for (i = esize; --i >= 0;) {
                gs_const_string rstr;

                gs_c_glyph_name(gs_c_known_encode((gs_char)i, index), &rstr);
                if (rstr.size == fstrs[i].size &&
                    !memcmp(rstr.data, fstrs[i].data, rstr.size)
                    )
                    continue;
                if (--match <= best)
                    break;
            }
            if (match > best) {
                best = match;
                near_index = index;
                /* If we have a perfect match, stop now. */
                if (best == esize)
                    break;
            }
        }
        index = near_index;
        if (best == esize)
            pfont->encoding_index = index;
    }
    pfont->nearest_encoding_index = index;
}