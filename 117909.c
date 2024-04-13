static boolean EmptyOutputBuffer(j_compress_ptr cinfo)
{
  DestinationManager
    *destination;

  destination=(DestinationManager *) cinfo->dest;
  destination->manager.free_in_buffer=(size_t) WriteBlob(destination->image,
    MaxBufferExtent,destination->buffer);
  if (destination->manager.free_in_buffer != MaxBufferExtent)
    ERREXIT(cinfo,JERR_FILE_WRITE);
  destination->manager.next_output_byte=destination->buffer;
  return(TRUE);
}