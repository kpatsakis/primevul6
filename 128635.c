build_proc_name_refs(const gs_memory_t *mem, build_proc_refs * pbuild,
                     const char *bcstr, const char *bgstr)
{
    int code;

    if (!bcstr)
        make_null(&pbuild->BuildChar);
    else {
        if ((code = name_ref(mem, (const byte *)bcstr,
                             strlen(bcstr), &pbuild->BuildChar, 0)) < 0)
            return code;
        r_set_attrs(&pbuild->BuildChar, a_executable);
    }
    if (!bgstr)
        make_null(&pbuild->BuildGlyph);
    else {
        if ((code = name_ref(mem, (const byte *)bgstr,
                             strlen(bgstr), &pbuild->BuildGlyph, 0)) < 0)
            return code;
        r_set_attrs(&pbuild->BuildGlyph, a_executable);
    }
    return 0;
}