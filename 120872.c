create_alpha_image_from_image_alpha_channel(const std::shared_ptr<HeifPixelImage> image)
{
  // --- generate alpha image
  // TODO: can we directly code a monochrome image instead of the dummy color channels?

  int chroma_width  = (image->get_width() +1)/2;
  int chroma_height = (image->get_height()+1)/2;

  std::shared_ptr<HeifPixelImage> alpha_image = std::make_shared<HeifPixelImage>();
  alpha_image->create(image->get_width(), image->get_height(),
                      heif_colorspace_YCbCr, heif_chroma_420);
  alpha_image->copy_new_plane_from(image, heif_channel_Alpha, heif_channel_Y);
  alpha_image->fill_new_plane(heif_channel_Cb, 128, chroma_width, chroma_height);
  alpha_image->fill_new_plane(heif_channel_Cr, 128, chroma_width, chroma_height);

  return alpha_image;
}