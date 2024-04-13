copy_font_name(gs_font_name * pfstr, const ref * pfname)
{
    uint size = r_size(pfname);

    if (size > gs_font_name_max)
        size = gs_font_name_max;
    memcpy(&pfstr->chars[0], pfname->value.const_bytes, size);
    /* Following is only for debugging printout. */
    pfstr->chars[size] = 0;
    pfstr->size = size;
}