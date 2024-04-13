static unsigned checkColorValidity(LodePNGColorType colortype, unsigned bd) /*bd = bitdepth*/
{
  switch(colortype)
  {
    case 0: if(!(bd == 1 || bd == 2 || bd == 4 || bd == 8 || bd == 16)) return 37; break; /*grey*/
    case 2: if(!(                                 bd == 8 || bd == 16)) return 37; break; /*RGB*/
    case 3: if(!(bd == 1 || bd == 2 || bd == 4 || bd == 8            )) return 37; break; /*palette*/
    case 4: if(!(                                 bd == 8 || bd == 16)) return 37; break; /*grey + alpha*/
    case 6: if(!(                                 bd == 8 || bd == 16)) return 37; break; /*RGBA*/
    default: return 31;
  }
  return 0; /*allowed color type / bits combination*/
}