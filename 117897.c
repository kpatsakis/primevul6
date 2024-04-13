static void JPEGSourceManager(j_decompress_ptr cinfo,Image *image)
{
  SourceManager
    *source;

  cinfo->src=(struct jpeg_source_mgr *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo,JPOOL_IMAGE,sizeof(SourceManager));
  source=(SourceManager *) cinfo->src;
  source->buffer=(JOCTET *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo,JPOOL_IMAGE,MaxBufferExtent*sizeof(JOCTET));
  source=(SourceManager *) cinfo->src;
  source->manager.init_source=InitializeSource;
  source->manager.fill_input_buffer=FillInputBuffer;
  source->manager.skip_input_data=SkipInputData;
  source->manager.resync_to_restart=jpeg_resync_to_restart;
  source->manager.term_source=TerminateSource;
  source->manager.bytes_in_buffer=0;
  source->manager.next_input_byte=NULL;
  source->image=image;
}