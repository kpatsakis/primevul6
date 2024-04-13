static unsigned getNumColorChannels(LodePNGColorType colortype)
{
  switch(colortype)
  {
    case 0: return 1; /*grey*/
    case 2: return 3; /*RGB*/
    case 3: return 1; /*palette*/
    case 4: return 2; /*grey + alpha*/
    case 6: return 4; /*RGBA*/
  }
  return 0; /*unexisting color type*/
}