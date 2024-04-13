static void InitializeDestination(j_compress_ptr cinfo)
{
  DestinationManager
    *destination;

  destination=(DestinationManager *) cinfo->dest;
  destination->buffer=(JOCTET *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo,JPOOL_IMAGE,MaxBufferExtent*sizeof(JOCTET));
  destination->manager.next_output_byte=destination->buffer;
  destination->manager.free_in_buffer=MaxBufferExtent;
}