void Magick::Image::adaptiveResize(const Geometry &geometry_)
{
  MagickCore::Image
    *newImage;

  size_t
    height=rows(),
    width=columns();

  ssize_t
    x=0,
    y=0;

  ParseMetaGeometry(static_cast<std::string>(geometry_).c_str(),&x,&y,&width,
    &height);

  GetPPException;
  newImage=AdaptiveResizeImage(constImage(),width,height,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}