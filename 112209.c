static unsigned short **AcquirePixelThreadSet(const size_t columns,
  const size_t channels)
{
  register ssize_t
    i;

  unsigned short
    **pixels;

  size_t
    number_threads;

  number_threads=(size_t) GetMagickResourceLimit(ThreadResource);
  pixels=(unsigned short **) AcquireQuantumMemory(number_threads,
    sizeof(*pixels));
  if (pixels == (unsigned short **) NULL)
    return((unsigned short **) NULL);
  (void) ResetMagickMemory(pixels,0,number_threads*sizeof(*pixels));
  for (i=0; i < (ssize_t) number_threads; i++)
  {
    pixels[i]=(unsigned short *) AcquireQuantumMemory(columns,channels*
      sizeof(**pixels));
    if (pixels[i] == (unsigned short *) NULL)
      return(DestroyPixelThreadSet(pixels));
  }
  return(pixels);
}