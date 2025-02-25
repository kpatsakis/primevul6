static char *TraceSVGClippath(const unsigned char *blob,size_t length,
  const size_t columns,const size_t rows)
{
  char
    *path,
    *message;

  MagickBooleanType
    in_subpath;

  PointInfo
    first[3],
    last[3],
    point[3];

  ssize_t
    i;

  ssize_t
    knot_count,
    selector,
    x,
    y;

  path=AcquireString((char *) NULL);
  if (path == (char *) NULL)
    return((char *) NULL);
  message=AcquireString((char *) NULL);
  (void) FormatLocaleString(message,MaxTextExtent,(
    "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
    "<svg xmlns=\"http://www.w3.org/2000/svg\""
    " width=\"%.20g\" height=\"%.20g\">\n"
    "<g>\n"
    "<path fill-rule=\"evenodd\" style=\"fill:#000000;stroke:#000000;"
    "stroke-width:0;stroke-antialiasing:false\" d=\"\n"),(double) columns,
    (double) rows);
  (void) ConcatenateString(&path,message);
  (void) memset(point,0,sizeof(point));
  (void) memset(first,0,sizeof(first));
  (void) memset(last,0,sizeof(last));
  knot_count=0;
  in_subpath=MagickFalse;
  while (length != 0)
  {
    selector=(ssize_t) ReadPropertyMSBShort(&blob,&length);
    switch (selector)
    {
      case 0:
      case 3:
      {
        if (knot_count != 0)
          {
            blob+=24;
            length-=MagickMin(24,(ssize_t) length);
            break;
          }
        /*
          Expected subpath length record.
        */
        knot_count=(ssize_t) ReadPropertyMSBShort(&blob,&length);
        blob+=22;
        length-=MagickMin(22,(ssize_t) length);
        break;
      }
      case 1:
      case 2:
      case 4:
      case 5:
      {
        if (knot_count == 0)
          {
            /*
              Unexpected subpath knot.
            */
            blob+=24;
            length-=MagickMin(24,(ssize_t) length);
            break;
          }
        /*
          Add sub-path knot.
        */
        for (i=0; i < 3; i++)
        {
          y=(ssize_t) ReadPropertyMSBLong(&blob,&length);
          x=(ssize_t) ReadPropertyMSBLong(&blob,&length);
          point[i].x=(double) x*columns/4096.0/4096.0;
          point[i].y=(double) y*rows/4096.0/4096.0;
        }
        if (in_subpath == MagickFalse)
          {
            (void) FormatLocaleString(message,MaxTextExtent,"M %g %g\n",
              point[1].x,point[1].y);
            for (i=0; i < 3; i++)
            {
              first[i]=point[i];
              last[i]=point[i];
            }
          }
        else
          {
            TraceBezierCurve(message,last,point);
            for (i=0; i < 3; i++)
              last[i]=point[i];
          }
        (void) ConcatenateString(&path,message);
        in_subpath=MagickTrue;
        knot_count--;
        /*
          Close the subpath if there are no more knots.
        */
        if (knot_count == 0)
          {
            TraceBezierCurve(message,last,first);
            (void) ConcatenateString(&path,message);
            in_subpath=MagickFalse;
          }
        break;
      }
      case 6:
      case 7:
      case 8:
      default:
      {
        blob+=24;
        length-=MagickMin(24,(ssize_t) length);
        break;
      }
    }
  }
  /*
    Return an empty SVG image if the path does not have knots.
  */
  (void) ConcatenateString(&path,"\"/>\n</g>\n</svg>\n");
  message=DestroyString(message);
  return(path);
}