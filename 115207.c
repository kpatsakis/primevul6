Magick_ColorType_from_PNG_ColorType(const int ping_colortype)
{
  switch (ping_colortype)
  {
    case 0:
      return "Grayscale";

    case 2:
      return "Truecolor";

    case 3:
      return "Indexed";

    case 4:
      return "GrayAlpha";

    case 6:
      return "RGBA";

    default:
      return "UndefinedColorType";
    }
}