static MagickBooleanType ReadPSDLayersInternal(Image *image,
  const ImageInfo *image_info,const PSDInfo *psd_info,
  const MagickBooleanType skip_layers,ExceptionInfo *exception)
{
  char
    type[4];

  LayerInfo
    *layer_info;

  MagickSizeType
    size;

  MagickBooleanType
    status;

  ssize_t
    i;

  ssize_t
    count,
    j,
    number_layers;

  size=GetLayerInfoSize(psd_info,image);
  status=MagickTrue;
  if (size != 0)
    {
      layer_info=(LayerInfo *) NULL;
      number_layers=(ssize_t) ReadBlobSignedShort(image);

      if (number_layers < 0)
        {
          /*
            The first alpha channel in the merged result contains the
            transparency data for the merged result.
          */
          number_layers=MagickAbsoluteValue(number_layers);
          if (image->debug != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  negative layer count corrected for");
          image->matte=MagickTrue;
        }

      /*
        We only need to know if the image has an alpha channel
      */
      if (skip_layers != MagickFalse)
        return(MagickTrue);

      if (image->debug != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "  image contains %.20g layers",(double) number_layers);

      if (number_layers == 0)
        ThrowBinaryException(CorruptImageError,"InvalidNumberOfLayers",
          image->filename);

      layer_info=(LayerInfo *) AcquireQuantumMemory((size_t) number_layers,
        sizeof(*layer_info));
      if (layer_info == (LayerInfo *) NULL)
        {
          if (image->debug != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  allocation of LayerInfo failed");
          ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
            image->filename);
        }
      (void) memset(layer_info,0,(size_t) number_layers*sizeof(*layer_info));

      for (i=0; i < number_layers; i++)
      {
        ssize_t
          top,
          left,
          bottom,
          right;

        if (image->debug != MagickFalse)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "  reading layer #%.20g",(double) i+1);
        top=(ssize_t) ReadBlobSignedLong(image);
        left=(ssize_t) ReadBlobSignedLong(image);
        bottom=(ssize_t) ReadBlobSignedLong(image);
        right=(ssize_t) ReadBlobSignedLong(image);
        if ((right < left) || (bottom < top))
          {
            layer_info=DestroyLayerInfo(layer_info,number_layers);
            ThrowBinaryException(CorruptImageError,"ImproperImageHeader",
              image->filename);
          }
        layer_info[i].page.y=top;
        layer_info[i].page.x=left;
        layer_info[i].page.width=(size_t) (right-left);
        layer_info[i].page.height=(size_t) (bottom-top);
        layer_info[i].channels=ReadBlobShort(image);
        if (layer_info[i].channels > MaxPSDChannels)
          {
            layer_info=DestroyLayerInfo(layer_info,number_layers);
            ThrowBinaryException(CorruptImageError,"MaximumChannelsExceeded",
              image->filename);
          }
        if (image->debug != MagickFalse)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    offset(%.20g,%.20g), size(%.20g,%.20g), channels=%.20g",
            (double) layer_info[i].page.x,(double) layer_info[i].page.y,
            (double) layer_info[i].page.height,(double)
            layer_info[i].page.width,(double) layer_info[i].channels);
        for (j=0; j < (ssize_t) layer_info[i].channels; j++)
        {
          layer_info[i].channel_info[j].type=(short) ReadBlobShort(image);
          if ((layer_info[i].channel_info[j].type < -4) ||
              (layer_info[i].channel_info[j].type > 4))
            {
              layer_info=DestroyLayerInfo(layer_info,number_layers);
              ThrowBinaryException(CorruptImageError,"NoSuchImageChannel",
                image->filename);
            }
          layer_info[i].channel_info[j].size=(size_t) GetPSDSize(psd_info,
            image);
          if (image->debug != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    channel[%.20g]: type=%.20g, size=%.20g",(double) j,
              (double) layer_info[i].channel_info[j].type,
              (double) layer_info[i].channel_info[j].size);
        }
        if (CheckPSDChannels(image,psd_info,&layer_info[i]) == MagickFalse)
          {
            layer_info=DestroyLayerInfo(layer_info,number_layers);
            ThrowBinaryException(CorruptImageError,"ImproperImageHeader",
              image->filename);
          }
        count=ReadPSDString(image,type,4);
        if ((count != 4) || (LocaleNCompare(type,"8BIM",4) != 0))
          {
            if (image->debug != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "  layer type was %.4s instead of 8BIM", type);
            layer_info=DestroyLayerInfo(layer_info,number_layers);
            ThrowBinaryException(CorruptImageError,"ImproperImageHeader",
              image->filename);
          }
        count=ReadPSDString(image,layer_info[i].blendkey,4);
        if (count != 4)
          {
            layer_info=DestroyLayerInfo(layer_info,number_layers);
            ThrowBinaryException(CorruptImageError,"ImproperImageHeader",
              image->filename);
          }
        layer_info[i].opacity=(Quantum) ScaleCharToQuantum((unsigned char)
          ReadBlobByte(image));
        layer_info[i].clipping=(unsigned char) ReadBlobByte(image);
        layer_info[i].flags=(unsigned char) ReadBlobByte(image);
        layer_info[i].visible=!(layer_info[i].flags & 0x02);
        if (image->debug != MagickFalse)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "   blend=%.4s, opacity=%.20g, clipping=%s, flags=%d, visible=%s",
            layer_info[i].blendkey,(double) layer_info[i].opacity,
            layer_info[i].clipping ? "true" : "false",layer_info[i].flags,
            layer_info[i].visible ? "true" : "false");
        (void) ReadBlobByte(image);  /* filler */

        size=ReadBlobLong(image);
        if (size != 0)
          {
            MagickSizeType
              combined_length,
              length;

            if (image->debug != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "    layer contains additional info");
            length=ReadBlobLong(image);
            combined_length=length+4;
            if (length != 0)
              {
                /*
                  Layer mask info.
                */
                layer_info[i].mask.page.y=(ssize_t) ReadBlobSignedLong(image);
                layer_info[i].mask.page.x=(ssize_t) ReadBlobSignedLong(image);
                layer_info[i].mask.page.height=(size_t) (
                  ReadBlobSignedLong(image)-layer_info[i].mask.page.y);
                layer_info[i].mask.page.width=(size_t) (
                  ReadBlobSignedLong(image)-layer_info[i].mask.page.x);
                layer_info[i].mask.background=(unsigned char) ReadBlobByte(
                  image);
                layer_info[i].mask.flags=(unsigned char) ReadBlobByte(image);
                if (!(layer_info[i].mask.flags & 0x01))
                  {
                    layer_info[i].mask.page.y=layer_info[i].mask.page.y-
                      layer_info[i].page.y;
                    layer_info[i].mask.page.x=layer_info[i].mask.page.x-
                      layer_info[i].page.x;
                  }
                if (image->debug != MagickFalse)
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                    "      layer mask: offset(%.20g,%.20g), size(%.20g,%.20g), length=%.20g",
                    (double) layer_info[i].mask.page.x,(double)
                    layer_info[i].mask.page.y,(double) layer_info[i].mask.page.width,
                    (double) layer_info[i].mask.page.height,(double)
                    ((MagickOffsetType) length)-18);
                /*
                  Skip over the rest of the layer mask information.
                */
                if (DiscardBlobBytes(image,(MagickSizeType) (length-18)) == MagickFalse)
                  {
                    layer_info=DestroyLayerInfo(layer_info,number_layers);
                    ThrowBinaryException(CorruptImageError,"UnexpectedEndOfFile",
                      image->filename);
                  }
              }
            length=ReadBlobLong(image);
            combined_length+=length+4;
            if (length != 0)
              {
                /*
                  Layer blending ranges info.
                */
                if (image->debug != MagickFalse)
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                    "      layer blending ranges: length=%.20g",(double)
                    ((MagickOffsetType) length));
                if (DiscardBlobBytes(image,length) == MagickFalse)
                  {
                    layer_info=DestroyLayerInfo(layer_info,number_layers);
                    ThrowBinaryException(CorruptImageError,
                      "UnexpectedEndOfFile",image->filename);
                  }
              }
            /*
              Layer name.
            */
            length=(MagickSizeType) (unsigned char) ReadBlobByte(image);
            combined_length+=length+1;
            if (length > 0)
              (void) ReadBlob(image,(size_t) length++,layer_info[i].name);
            layer_info[i].name[length]='\0';
            if (image->debug != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "      layer name: %s",layer_info[i].name);
            if ((length % 4) != 0)
              {
                length=4-(length % 4);
                combined_length+=length;
                /* Skip over the padding of the layer name */
                if (DiscardBlobBytes(image,length) == MagickFalse)
                  {
                    layer_info=DestroyLayerInfo(layer_info,number_layers);
                    ThrowBinaryException(CorruptImageError,
                      "UnexpectedEndOfFile",image->filename);
                  }
              }
            length=(MagickSizeType) size-combined_length;
            if (length > 0)
              {
                unsigned char
                  *info;

                if (length > GetBlobSize(image))
                  {
                    layer_info=DestroyLayerInfo(layer_info,number_layers);
                    ThrowBinaryException(CorruptImageError,
                      "InsufficientImageDataInFile",image->filename);
                  }
                layer_info[i].info=AcquireStringInfo((const size_t) length);
                info=GetStringInfoDatum(layer_info[i].info);
                (void) ReadBlob(image,(const size_t) length,info);
              }
          }
      }

      for (i=0; i < number_layers; i++)
      {
        if ((layer_info[i].page.width == 0) ||
              (layer_info[i].page.height == 0))
          {
            if (image->debug != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "      layer data is empty");
            if (layer_info[i].info != (StringInfo *) NULL)
              layer_info[i].info=DestroyStringInfo(layer_info[i].info);
            continue;
          }
        /*
          Allocate layered image.
        */
        layer_info[i].image=CloneImage(image,layer_info[i].page.width,
          layer_info[i].page.height,MagickFalse,exception);
        if (layer_info[i].image == (Image *) NULL)
          {
            layer_info=DestroyLayerInfo(layer_info,number_layers);
            if (image->debug != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "  allocation of image for layer %.20g failed",(double) i);
            ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
              image->filename);
          }
        if (layer_info[i].info != (StringInfo *) NULL)
          {
            (void) SetImageProfile(layer_info[i].image,"psd:additional-info",
              layer_info[i].info);
            layer_info[i].info=DestroyStringInfo(layer_info[i].info);
          }
      }

      if (image_info->ping == MagickFalse)
        {
          for (i=0; i < number_layers; i++)
          {
            if (layer_info[i].image == (Image *) NULL)
              {
                for (j=0; j < (ssize_t) layer_info[i].channels; j++)
                {
                  if (DiscardBlobBytes(image,(MagickSizeType)
                      layer_info[i].channel_info[j].size) == MagickFalse)
                    {
                      layer_info=DestroyLayerInfo(layer_info,number_layers);
                      ThrowBinaryException(CorruptImageError,
                        "UnexpectedEndOfFile",image->filename);
                    }
                }
                continue;
              }

            if (image->debug != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "  reading data for layer %.20g",(double) i);
            status=ReadPSDLayer(image,image_info,psd_info,&layer_info[i],
              exception);
            if (status == MagickFalse)
              break;

            status=SetImageProgress(image,LoadImagesTag,(MagickOffsetType) i,
              (MagickSizeType) number_layers);
            if (status == MagickFalse)
              break;
          }
        }

      if (status != MagickFalse)
        {
          for (i=0; i < number_layers; i++)
          {
            if (layer_info[i].image == (Image *) NULL)
              {
                for (j=i; j < number_layers - 1; j++)
                  layer_info[j] = layer_info[j+1];
                number_layers--;
                i--;
              }
          }

          if (number_layers > 0)
            {
              for (i=0; i < number_layers; i++)
              {
                if (i > 0)
                  layer_info[i].image->previous=layer_info[i-1].image;
                if (i < (number_layers-1))
                  layer_info[i].image->next=layer_info[i+1].image;
                layer_info[i].image->page=layer_info[i].page;
              }
              image->next=layer_info[0].image;
              layer_info[0].image->previous=image;
            }
          layer_info=(LayerInfo *) RelinquishMagickMemory(layer_info);
        }
      else
        layer_info=DestroyLayerInfo(layer_info,number_layers);
    }

  return(status);
}