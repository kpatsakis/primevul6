void Magick::Image::floodFillTexture(const Magick::Geometry &point_,
  const Magick::Image &texture_,const Magick::Color &borderColor_,
  const bool invert_)
{
  floodFillTexture(point_.xOff(),point_.yOff(),texture_,borderColor_,invert_);
}