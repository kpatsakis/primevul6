void FrameFactory::setDefaultTextEncoding(String::Type encoding)
{
  d->useDefaultEncoding = true;
  d->defaultEncoding = encoding;
}