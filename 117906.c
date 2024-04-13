static void JPEGDestinationManager(j_compress_ptr cinfo,Image * image)
{
  DestinationManager
    *destination;

  cinfo->dest=(struct jpeg_destination_mgr *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo,JPOOL_IMAGE,sizeof(DestinationManager));
  destination=(DestinationManager *) cinfo->dest;
  destination->manager.init_destination=InitializeDestination;
  destination->manager.empty_output_buffer=EmptyOutputBuffer;
  destination->manager.term_destination=TerminateDestination;
  destination->image=image;
}