static inline MagickBooleanType IsPoint(const char *point)
{
  char
    *p;

  double
    value;

  value=GetDrawValue(point,&p);
  return((fabs(value) < MagickEpsilon) && (p == point) ? MagickFalse :
    MagickTrue);
}