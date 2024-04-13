static void AttachPSDLayers(Image *image,LayerInfo *layer_info,
  ssize_t number_layers)
{
  ssize_t
    i;

  ssize_t
    j;

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
  if (number_layers == 0)
    {
      layer_info=(LayerInfo *) RelinquishMagickMemory(layer_info);
      return;
    }
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
  layer_info=(LayerInfo *) RelinquishMagickMemory(layer_info);
}