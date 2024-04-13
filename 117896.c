static void SkipInputData(j_decompress_ptr cinfo,long number_bytes)
{
  SourceManager
    *source;

  if (number_bytes <= 0)
    return;
  source=(SourceManager *) cinfo->src;
  while (number_bytes > (long) source->manager.bytes_in_buffer)
  {
    number_bytes-=(long) source->manager.bytes_in_buffer;
    (void) FillInputBuffer(cinfo);
  }
  source->manager.next_input_byte+=number_bytes;
  source->manager.bytes_in_buffer-=number_bytes;
}