static int lodepng_add32bitInt(ucvector* buffer, unsigned value)
{
  if (!ucvector_resize(buffer, buffer->size + 4)) return 0;
  lodepng_set32bitInt(&buffer->data[buffer->size - 4], value);
  return 1;
}