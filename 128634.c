ENUM_PTRS_BEGIN_PROC(font_data_enum_ptrs)
{
    return ref_struct_enum_ptrs(mem, vptr, offset_of(font_data, u.type42.mru_sfnts_index)/*size*/, index, pep, pstype, gcst);
}