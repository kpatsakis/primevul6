static void TerminateDestination(j_compress_ptr cinfo)
{
  DestinationManager
    *destination;

  destination=(DestinationManager *) cinfo->dest;
  if ((MaxBufferExtent-(int) destination->manager.free_in_buffer) > 0)
    {
      ssize_t
        count;

      count=WriteBlob(destination->image,MaxBufferExtent-
        destination->manager.free_in_buffer,destination->buffer);
      if (count != (ssize_t)
          (MaxBufferExtent-destination->manager.free_in_buffer))
        ERREXIT(cinfo,JERR_FILE_WRITE);
    }
}