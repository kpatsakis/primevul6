static inline LayerInfo *DestroyLayerInfo(LayerInfo *layer_info,
  const ssize_t number_layers)
{
  ssize_t
    i;

  for (i=0; i<number_layers; i++)
  {
    if (layer_info[i].image != (Image *) NULL)
      layer_info[i].image=DestroyImage(layer_info[i].image);
    if (layer_info[i].mask.image != (Image *) NULL)
      layer_info[i].mask.image=DestroyImage(layer_info[i].mask.image);
  }

  return (LayerInfo *) RelinquishMagickMemory(layer_info);
}