zfont_encode_char(gs_font *pfont, gs_char chr, gs_glyph_space_t gspace)
{
    font_data *pdata = pfont_data(pfont);
    const ref *pencoding = &pdata->Encoding;
    ulong index = chr;	/* work around VAX widening bug */
    ref cname;
    int code = array_get(pfont->memory, pencoding, (long)index, &cname);

    if (code < 0 || !r_has_type(&cname, t_name))
        return GS_NO_GLYPH;
    if (pfont->FontType == ft_user_defined && r_type(&pdata->BuildGlyph) == t_null) {
        ref nsref, tname;

        name_string_ref(pfont->memory, &cname, &nsref);
        if (r_size(&nsref) == 7 &&
            !memcmp(nsref.value.const_bytes, ".notdef", r_size(&nsref))) {
            /* A special support for high level devices.
               They need a glyph name but the font doesn't provide one
               due to an instandard BuildChar.
               Such fonts don't conform to PLRM section 5.3.7,
               but we've got real examples that we want to handle (Bug 686982).
               Construct a name here.
               Low level devices don't pass here, because regular PS interpretation
               doesn't need such names.
            */
            char buf[20];
            int code;

            if (gspace == GLYPH_SPACE_NOGEN)
                return GS_NO_GLYPH;
            gs_sprintf(buf, "j%ld", chr); /* 'j' is arbutrary. */
            code = name_ref(pfont->memory, (const byte *)buf, strlen(buf), &tname, 1);
            if (code < 0) {
                /* Can't propagate the error due to interface limitation,
                   return with .notdef */
            } else
                cname = tname;
        }
    }
    return (gs_glyph)name_index(pfont->memory, &cname);
}