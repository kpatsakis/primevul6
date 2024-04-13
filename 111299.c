void LineCompositeTask::execute()
{
  composite_line(_y,_start,_Data,*_names,*_pointers,*_total_sizes,*_num_sources);
}