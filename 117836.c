static void reschedule_stream(nghttp2_stream *stream) {
  stream->last_writelen = stream->item->frame.hd.length;

  nghttp2_stream_reschedule(stream);
}