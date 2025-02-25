ModuleExport MagickBooleanType ReadPSDLayers(Image *image,
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

  register ssize_t
    i;

  ssize_t
    count,
    j,
    number_layers;

  size=GetPSDSize(psd_info,image);
  if (size == 0)
    {
      /*
        Skip layers & masks.
      */
      (void) ReadBlobLong(image);
      count=ReadBlob(image,4,(unsigned char *) type);
      ReversePSDString(image,type,4);
      status=MagickFalse;
      if ((count == 0) || (LocaleNCompare(type,"8BIM",4) != 0))
        return(MagickTrue);
      else
        {
          count=ReadBlob(image,4,(unsigned char *) type);
          ReversePSDString(image,type,4);
          if ((count != 0) && (LocaleNCompare(type,"Lr16",4) == 0))
            size=GetPSDSize(psd_info,image);
          else
            return(MagickTrue);
        }
    }
  status=MagickTrue;
  if (size != 0)
    {
      layer_info=(LayerInfo *) NULL;
      number_layers=(short) ReadBlobShort(image);

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
          image->alpha_trait=BlendPixelTrait;
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
      (void) ResetMagickMemory(layer_info,0,(size_t) number_layers*
        sizeof(*layer_info));

      for (i=0; i < number_layers; i++)
      {
        ssize_t
          x,
          y;

        if (image->debug != MagickFalse)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "  reading layer #%.20g",(double) i+1);
        layer_info[i].page.y=(int) ReadBlobLong(image);
        layer_info[i].page.x=(int) ReadBlobLong(image);
        y=(int) ReadBlobLong(image);
        x=(int) ReadBlobLong(image);
        layer_info[i].page.width=(size_t) (x-layer_info[i].page.x);
        layer_info[i].page.height=(size_t) (y-layer_info[i].page.y);
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
          layer_info[i].channel_info[j].size=(size_t) GetPSDSize(psd_info,
            image);
          if (image->debug != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    channel[%.20g]: type=%.20g, size=%.20g",(double) j,
              (double) layer_info[i].channel_info[j].type,
              (double) layer_info[i].channel_info[j].size);
        }
        count=ReadBlob(image,4,(unsigned char *) type);
        ReversePSDString(image,type,4);
        if ((count == 0) || (LocaleNCompare(type,"8BIM",4) != 0))
          {
            if (image->debug != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "  layer type was %.4s instead of 8BIM", type);
            layer_info=DestroyLayerInfo(layer_info,number_layers);
            ThrowBinaryException(CorruptImageError,"ImproperImageHeader",
              image->filename);
          }
        count=ReadBlob(image,4,(unsigned char *) layer_info[i].blendkey);
        ReversePSDString(image,layer_info[i].blendkey,4);
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
                layer_info[i].mask.page.y=(int) ReadBlobLong(image);
                layer_info[i].mask.page.x=(int) ReadBlobLong(image);
                layer_info[i].mask.page.height=(size_t) (ReadBlobLong(image)-
                  layer_info[i].mask.page.y);
                layer_info[i].mask.page.width=(size_t) (ReadBlobLong(image)-
                  layer_info[i].mask.page.x);
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
                /*
                  We read it, but don't use it...
                */
                for (j=0; j < (ssize_t) (length); j+=8)
                {
                  size_t blend_source=ReadBlobLong(image);
                  size_t blend_dest=ReadBlobLong(image);
                  if (image->debug != MagickFalse)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                      "        source(%x), dest(%x)",(unsigned int)
                      blend_source,(unsigned int) blend_dest);
                }
              }
            /*
              Layer name.
            */
            length=(size_t) ReadBlobByte(image);
            combined_length+=length+1;
            if (length > 0)
              (void) ReadBlob(image,(size_t) length++,layer_info[i].name);
            layer_info[i].name[length]='\0';
            if (image->debug != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "      layer name: %s",layer_info[i].name);
            /*
               Skip the rest of the variable data until we support it.
             */
             if (image->debug != MagickFalse)
               (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "      unsupported data: length=%.20g",(double)
                 ((MagickOffsetType) (size-combined_length)));
             if (DiscardBlobBytes(image,(MagickSizeType) (size-combined_length)) == MagickFalse)
               {
                 layer_info=DestroyLayerInfo(layer_info,number_layers);
                 ThrowBinaryException(CorruptImageError,
                   "UnexpectedEndOfFile",image->filename);
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
      }

      if (image_info->ping == MagickFalse)
        {
          for (i=0; i < number_layers; i++)
          {
            if (layer_info[i].image == (Image *) NULL)
              {
                for (j=0; j < layer_info[i].channels; j++)
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

            status=ReadPSDLayer(image,psd_info,&layer_info[i],exception);
            if (status == MagickFalse)
              break;

            status=SetImageProgress(image,LoadImagesTag,i,(MagickSizeType)
              number_layers);
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