get_font_name(const gs_memory_t *mem, ref * pfname, const ref * op)
{
    switch (r_type(op)) {
        case t_string:
            *pfname = *op;
            break;
        case t_name:
            name_string_ref(mem, op, pfname);
            break;
        default:
            /* This is weird, but legal.... */
            make_empty_string(pfname, a_readonly);
    }
}