build_gs_font_procs(os_ptr op, build_proc_refs * pbuild)
{
    int ccode, gcode;
    ref *pBuildChar;
    ref *pBuildGlyph;

    check_type(*op, t_dictionary);
    ccode = dict_find_string(op, "BuildChar", &pBuildChar);
    gcode = dict_find_string(op, "BuildGlyph", &pBuildGlyph);
    if (ccode <= 0) {
        if (gcode <= 0)
            return_error(gs_error_invalidfont);
        make_null(&pbuild->BuildChar);
    } else {
        check_proc(*pBuildChar);
        pbuild->BuildChar = *pBuildChar;
    }
    if (gcode <= 0)
        make_null(&pbuild->BuildGlyph);
    else {
        check_proc(*pBuildGlyph);
        pbuild->BuildGlyph = *pBuildGlyph;
    }
    return 0;
}