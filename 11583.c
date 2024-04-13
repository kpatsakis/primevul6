check_printer(TERMTYPE2 *tp)
{
    (void) tp;
#if defined(enter_doublewide_mode) && defined(exit_doublewide_mode)
    PAIRED(enter_doublewide_mode, exit_doublewide_mode);
#endif
#if defined(enter_italics_mode) && defined(exit_italics_mode)
    PAIRED(enter_italics_mode, exit_italics_mode);
#endif
#if defined(enter_leftward_mode) && defined(exit_leftward_mode)
    PAIRED(enter_leftward_mode, exit_leftward_mode);
#endif
#if defined(enter_micro_mode) && defined(exit_micro_mode)
    PAIRED(enter_micro_mode, exit_micro_mode);
#endif
#if defined(enter_shadow_mode) && defined(exit_shadow_mode)
    PAIRED(enter_shadow_mode, exit_shadow_mode);
#endif
#if defined(enter_subscript_mode) && defined(exit_subscript_mode)
    PAIRED(enter_subscript_mode, exit_subscript_mode);
#endif
#if defined(enter_superscript_mode) && defined(exit_superscript_mode)
    PAIRED(enter_superscript_mode, exit_superscript_mode);
#endif
#if defined(enter_upward_mode) && defined(exit_upward_mode)
    PAIRED(enter_upward_mode, exit_upward_mode);
#endif

#if defined(start_char_set_def) && defined(stop_char_set_def)
    ANDMISSING(start_char_set_def, stop_char_set_def);
#endif

    /* if we have a parameterized form, then the non-parameterized is easy */
#if defined(set_bottom_margin_parm) && defined(set_bottom_margin)
    ANDMISSING(set_bottom_margin_parm, set_bottom_margin);
#endif
#if defined(set_left_margin_parm) && defined(set_left_margin)
    ANDMISSING(set_left_margin_parm, set_left_margin);
#endif
#if defined(set_right_margin_parm) && defined(set_right_margin)
    ANDMISSING(set_right_margin_parm, set_right_margin);
#endif
#if defined(set_top_margin_parm) && defined(set_top_margin)
    ANDMISSING(set_top_margin_parm, set_top_margin);
#endif

#if defined(parm_down_micro) && defined(micro_down)
    ANDMISSING(parm_down_micro, micro_down);
#endif
#if defined(parm_left_micro) && defined(micro_left)
    ANDMISSING(parm_left_micro, micro_left);
#endif
#if defined(parm_right_micro) && defined(micro_right)
    ANDMISSING(parm_right_micro, micro_right);
#endif
#if defined(parm_up_micro) && defined(micro_up)
    ANDMISSING(parm_up_micro, micro_up);
#endif
}