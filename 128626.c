CLEAR_MARKS_PROC(font_data_clear_marks)
{
    ref_struct_clear_marks(cmem, vptr, offset_of(font_data, u.type42.mru_sfnts_index)/*size*/, pstype);
}