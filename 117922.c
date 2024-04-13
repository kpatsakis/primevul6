static void InitializeSource(j_decompress_ptr cinfo)
{
  SourceManager
    *source;

  source=(SourceManager *) cinfo->src;
  source->start_of_blob=TRUE;
}