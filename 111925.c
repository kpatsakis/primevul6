static inline void SetPSDPixel(Image *image,const PixelChannel channel,
  const size_t packet_size,const Quantum pixel,Quantum *q,
  ExceptionInfo *exception)
{
  if (image->storage_class == PseudoClass)
    {
      PixelInfo
        *color;

      ssize_t
        index;

      if (channel == GrayPixelChannel)
        {
          index=(ssize_t) pixel;
          if (packet_size == 1)
            index=(ssize_t) ScaleQuantumToChar((Quantum) index);
          index=ConstrainColormapIndex(image,index,exception);
          SetPixelIndex(image,(Quantum) index,q);
        }
      else
        {
          index=(ssize_t) GetPixelIndex(image,q);
          index=ConstrainColormapIndex(image,index,exception);
        }
      color=image->colormap+index;
      if (channel == AlphaPixelChannel)
        color->alpha=(MagickRealType) pixel;
      SetPixelViaPixelInfo(image,color,q);
    }
  else
    SetPixelChannel(image,channel,pixel,q);
}