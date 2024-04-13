static int font_with_same_UID_and_another_metrics(const gs_font *pfont0, const gs_font *pfont1)
{
    const gs_font_base *pbfont0 = (const gs_font_base *)pfont0;
    const gs_font_base *pbfont1 = (const gs_font_base *)pfont1;

    if (uid_equal(&pbfont0->UID, &pbfont1->UID)) {
        const ref *pfdict0 = &pfont_data(gs_font_parent(pbfont0))->dict;
        const ref *pfdict1 = &pfont_data(gs_font_parent(pbfont1))->dict;
        ref *pmdict0, *pmdict1;

        if (pbfont0->WMode || dict_find_string(pfdict0, "Metrics", &pmdict0) <= 0)
            pmdict0 = NULL;
        if (pbfont1->WMode || dict_find_string(pfdict1, "Metrics", &pmdict1) <= 0)
            pmdict1 = NULL;
        if (!pmdict0 != !pmdict1)
            return 1;
        if (pmdict0 != NULL && !obj_eq(pfont0->memory, pmdict0, pmdict1))
            return 1;
        if (!pbfont0->WMode || dict_find_string(pfdict0, "Metrics2", &pmdict0) <= 0)
            pmdict0 = NULL;
        if (!pbfont0->WMode || dict_find_string(pfdict1, "Metrics2", &pmdict1) <= 0)
            pmdict1 = NULL;
        if (!pmdict0 != !pmdict1)
            return 1;
        if (pmdict0 != NULL && !obj_eq(pfont0->memory, pmdict0, pmdict1))
            return 1;
    }
    return 0;
}