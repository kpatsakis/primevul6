static boolean FillInputBuffer(j_decompress_ptr cinfo)
{
  SourceManager
    *source;

  source=(SourceManager *) cinfo->src;
  source->manager.bytes_in_buffer=(size_t) ReadBlob(source->image,
    MaxBufferExtent,source->buffer);
  if (source->manager.bytes_in_buffer == 0)
    {
      if (source->start_of_blob != FALSE)
        ERREXIT(cinfo,JERR_INPUT_EMPTY);
      WARNMS(cinfo,JWRN_JPEG_EOF);
      source->buffer[0]=(JOCTET) 0xff;
      source->buffer[1]=(JOCTET) JPEG_EOI;
      source->manager.bytes_in_buffer=2;
    }
  source->manager.next_input_byte=source->buffer;
  source->start_of_blob=FALSE;
  return(TRUE);
}