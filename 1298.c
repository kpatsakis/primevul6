static inline void SetPSDPixel(Image *image,const size_t channels,
  const ssize_t type,const size_t packet_size,const Quantum pixel,PixelPacket *q,
  IndexPacket *indexes,ssize_t x)
{
  if (image->storage_class == PseudoClass)
    {
      PixelPacket
        *color;

      IndexPacket
        index;

      index=(IndexPacket) pixel;
      if (packet_size == 1)
        index=(IndexPacket) ScaleQuantumToChar(index);
      index=ConstrainColormapIndex(image,(ssize_t) index);

      if (type == 0)
        SetPixelIndex(indexes+x,index);
      if ((type == 0) && (channels > 1))
        return;
      color=image->colormap+(ssize_t) GetPixelIndex(indexes+x);
      if (type != 0)
        SetPixelAlpha(color,pixel);
      SetPixelRGBO(q,color);
      return;
    }
  switch (type)
  {
    case -1:
    {
      SetPixelAlpha(q,pixel);
      break;
    }
    case -2:
    case 0:
    {
      SetPixelRed(q,pixel);
      if ((channels < 3) || (type == -2))
        {
          SetPixelGreen(q,GetPixelRed(q));
          SetPixelBlue(q,GetPixelRed(q));
        }
      break;
    }
    case -3:
    case 1:
    {
      SetPixelGreen(q,pixel);
      break;
    }
    case -4:
    case 2:
    {
      SetPixelBlue(q,pixel);
      break;
    }
    case 3:
    {
      if (image->colorspace == CMYKColorspace)
        SetPixelIndex(indexes+x,pixel);
      else
        if (image->matte != MagickFalse)
          SetPixelAlpha(q,pixel);
      break;
    }
    case 4:
    {
      if ((IssRGBCompatibleColorspace(image->colorspace) != MagickFalse) &&
          (channels > 3))
        break;
      if (image->matte != MagickFalse)
        SetPixelAlpha(q,pixel);
      break;
    }
  }
}