zfont_glyph_name(gs_font *font, gs_glyph index, gs_const_string *pstr)
{
    ref nref, sref;

    if (index >= GS_MIN_CID_GLYPH) {	/* Fabricate a numeric name. */
        char cid_name[sizeof(gs_glyph) * 3 + 1];
        int code;

        gs_sprintf(cid_name, "%lu", (ulong) index);
        code = name_ref(font->memory, (const byte *)cid_name, strlen(cid_name),
                        &nref, 1);
        if (code < 0)
            return code;
    } else
        name_index_ref(font->memory, index, &nref);
    name_string_ref(font->memory, &nref, &sref);
    pstr->data = sref.value.const_bytes;
    pstr->size = r_size(&sref);
    return 0;
}