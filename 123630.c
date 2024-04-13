static ssize_t TracePath(MVGInfo *mvg_info,const char *path,
  ExceptionInfo *exception)
{
  char
    *next_token,
    token[MagickPathExtent];

  const char
    *p;

  double
    x,
    y;

  int
    attribute,
    last_attribute;

  MagickBooleanType
    status;

  PointInfo
    end = {0.0, 0.0},
    points[4] = { {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0} },
    point = {0.0, 0.0},
    start = {0.0, 0.0};

  PrimitiveInfo
    *primitive_info;

  PrimitiveType
    primitive_type;

  PrimitiveInfo
    *q;

  ssize_t
    i;

  size_t
    number_coordinates,
    z_count;

  ssize_t
    subpath_offset;

  subpath_offset=mvg_info->offset;
  primitive_info=(*mvg_info->primitive_info)+mvg_info->offset;
  status=MagickTrue;
  attribute=0;
  number_coordinates=0;
  z_count=0;
  primitive_type=primitive_info->primitive;
  q=primitive_info;
  for (p=path; *p != '\0'; )
  {
    if (status == MagickFalse)
      break;
    while (isspace((int) ((unsigned char) *p)) != 0)
      p++;
    if (*p == '\0')
      break;
    last_attribute=attribute;
    attribute=(int) (*p++);
    switch (attribute)
    {
      case 'a':
      case 'A':
      {
        double
          angle = 0.0;

        MagickBooleanType
          large_arc = MagickFalse,
          sweep = MagickFalse;

        PointInfo
          arc = {0.0, 0.0};

        /*
          Elliptical arc.
        */
        do
        {
          (void) GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            (void) GetNextToken(p,&p,MagickPathExtent,token);
          arc.x=GetDrawValue(token,&next_token);
          if (token == next_token)
            ThrowPointExpectedException(token,exception);
          (void) GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            (void) GetNextToken(p,&p,MagickPathExtent,token);
          arc.y=GetDrawValue(token,&next_token);
          if (token == next_token)
            ThrowPointExpectedException(token,exception);
          (void) GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            (void) GetNextToken(p,&p,MagickPathExtent,token);
          angle=GetDrawValue(token,&next_token);
          if (token == next_token)
            ThrowPointExpectedException(token,exception);
          (void) GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            (void) GetNextToken(p,&p,MagickPathExtent,token);
          large_arc=StringToLong(token) != 0 ? MagickTrue : MagickFalse;
          (void) GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            (void) GetNextToken(p,&p,MagickPathExtent,token);
          sweep=StringToLong(token) != 0 ? MagickTrue : MagickFalse;
          if (*token == ',')
            (void) GetNextToken(p,&p,MagickPathExtent,token);
          (void) GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            (void) GetNextToken(p,&p,MagickPathExtent,token);
          x=GetDrawValue(token,&next_token);
          if (token == next_token)
            ThrowPointExpectedException(token,exception);
          (void) GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            (void) GetNextToken(p,&p,MagickPathExtent,token);
          y=GetDrawValue(token,&next_token);
          if (token == next_token)
            ThrowPointExpectedException(token,exception);
          end.x=(double) (attribute == (int) 'A' ? x : point.x+x);
          end.y=(double) (attribute == (int) 'A' ? y : point.y+y);
          if (TraceArcPath(mvg_info,point,end,arc,angle,large_arc,sweep) == MagickFalse)
            return(-1);
          q=(*mvg_info->primitive_info)+mvg_info->offset;
          mvg_info->offset+=q->coordinates;
          q+=q->coordinates;
          point=end;
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 'c':
      case 'C':
      {
        /*
          Cubic Bézier curve.
        */
        do
        {
          points[0]=point;
          for (i=1; i < 4; i++)
          {
            (void) GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              (void) GetNextToken(p,&p,MagickPathExtent,token);
            x=GetDrawValue(token,&next_token);
            if (token == next_token)
              ThrowPointExpectedException(token,exception);
            (void) GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              (void) GetNextToken(p,&p,MagickPathExtent,token);
            y=GetDrawValue(token,&next_token);
            if (token == next_token)
              ThrowPointExpectedException(token,exception);
            end.x=(double) (attribute == (int) 'C' ? x : point.x+x);
            end.y=(double) (attribute == (int) 'C' ? y : point.y+y);
            points[i]=end;
          }
          for (i=0; i < 4; i++)
            (q+i)->point=points[i];
          if (TraceBezier(mvg_info,4) == MagickFalse)
            return(-1);
          q=(*mvg_info->primitive_info)+mvg_info->offset;
          mvg_info->offset+=q->coordinates;
          q+=q->coordinates;
          point=end;
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 'H':
      case 'h':
      {
        do
        {
          (void) GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            (void) GetNextToken(p,&p,MagickPathExtent,token);
          x=GetDrawValue(token,&next_token);
          if (token == next_token)
            ThrowPointExpectedException(token,exception);
          point.x=(double) (attribute == (int) 'H' ? x: point.x+x);
          if (CheckPrimitiveExtent(mvg_info,PrimitiveExtentPad) == MagickFalse)
            return(-1);
          q=(*mvg_info->primitive_info)+mvg_info->offset;
          if (TracePoint(q,point) == MagickFalse)
            return(-1);
          mvg_info->offset+=q->coordinates;
          q+=q->coordinates;
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 'l':
      case 'L':
      {
        /*
          Line to.
        */
        do
        {
          (void) GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            (void) GetNextToken(p,&p,MagickPathExtent,token);
          x=GetDrawValue(token,&next_token);
          if (token == next_token)
            ThrowPointExpectedException(token,exception);
          (void) GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            (void) GetNextToken(p,&p,MagickPathExtent,token);
          y=GetDrawValue(token,&next_token);
          if (token == next_token)
            ThrowPointExpectedException(token,exception);
          point.x=(double) (attribute == (int) 'L' ? x : point.x+x);
          point.y=(double) (attribute == (int) 'L' ? y : point.y+y);
          if (CheckPrimitiveExtent(mvg_info,PrimitiveExtentPad) == MagickFalse)
            return(-1);
          q=(*mvg_info->primitive_info)+mvg_info->offset;
          if (TracePoint(q,point) == MagickFalse)
            return(-1);
          mvg_info->offset+=q->coordinates;
          q+=q->coordinates;
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 'M':
      case 'm':
      {
        /*
          Move to.
        */
        if (mvg_info->offset != subpath_offset)
          {
            primitive_info=(*mvg_info->primitive_info)+subpath_offset;
            primitive_info->coordinates=(size_t) (q-primitive_info);
            number_coordinates+=primitive_info->coordinates;
            primitive_info=q;
            subpath_offset=mvg_info->offset;
          }
        i=0;
        do
        {
          (void) GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            (void) GetNextToken(p,&p,MagickPathExtent,token);
          x=GetDrawValue(token,&next_token);
          if (token == next_token)
            ThrowPointExpectedException(token,exception);
          (void) GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            (void) GetNextToken(p,&p,MagickPathExtent,token);
          y=GetDrawValue(token,&next_token);
          if (token == next_token)
            ThrowPointExpectedException(token,exception);
          point.x=(double) (attribute == (int) 'M' ? x : point.x+x);
          point.y=(double) (attribute == (int) 'M' ? y : point.y+y);
          if (i == 0)
            start=point;
          i++;
          if (CheckPrimitiveExtent(mvg_info,PrimitiveExtentPad) == MagickFalse)
            return(-1);
          q=(*mvg_info->primitive_info)+mvg_info->offset;
          if (TracePoint(q,point) == MagickFalse)
            return(-1);
          mvg_info->offset+=q->coordinates;
          q+=q->coordinates;
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 'q':
      case 'Q':
      {
        /*
          Quadratic Bézier curve.
        */
        do
        {
          points[0]=point;
          for (i=1; i < 3; i++)
          {
            (void) GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              (void) GetNextToken(p,&p,MagickPathExtent,token);
            x=GetDrawValue(token,&next_token);
            if (token == next_token)
              ThrowPointExpectedException(token,exception);
            (void) GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              (void) GetNextToken(p,&p,MagickPathExtent,token);
            y=GetDrawValue(token,&next_token);
            if (token == next_token)
              ThrowPointExpectedException(token,exception);
            if (*p == ',')
              p++;
            end.x=(double) (attribute == (int) 'Q' ? x : point.x+x);
            end.y=(double) (attribute == (int) 'Q' ? y : point.y+y);
            points[i]=end;
          }
          for (i=0; i < 3; i++)
            (q+i)->point=points[i];
          if (TraceBezier(mvg_info,3) == MagickFalse)
            return(-1);
          q=(*mvg_info->primitive_info)+mvg_info->offset;
          mvg_info->offset+=q->coordinates;
          q+=q->coordinates;
          point=end;
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 's':
      case 'S':
      {
        /*
          Cubic Bézier curve.
        */
        do
        {
          points[0]=points[3];
          points[1].x=2.0*points[3].x-points[2].x;
          points[1].y=2.0*points[3].y-points[2].y;
          for (i=2; i < 4; i++)
          {
            (void) GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              (void) GetNextToken(p,&p,MagickPathExtent,token);
            x=GetDrawValue(token,&next_token);
            if (token == next_token)
              ThrowPointExpectedException(token,exception);
            (void) GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              (void) GetNextToken(p,&p,MagickPathExtent,token);
            y=GetDrawValue(token,&next_token);
            if (token == next_token)
              ThrowPointExpectedException(token,exception);
            if (*p == ',')
              p++;
            end.x=(double) (attribute == (int) 'S' ? x : point.x+x);
            end.y=(double) (attribute == (int) 'S' ? y : point.y+y);
            points[i]=end;
          }
          if (strchr("CcSs",last_attribute) == (char *) NULL)
            {
              points[0]=point;
              points[1]=point;
            }
          for (i=0; i < 4; i++)
            (q+i)->point=points[i];
          if (TraceBezier(mvg_info,4) == MagickFalse)
            return(-1);
          q=(*mvg_info->primitive_info)+mvg_info->offset;
          mvg_info->offset+=q->coordinates;
          q+=q->coordinates;
          point=end;
          last_attribute=attribute;
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 't':
      case 'T':
      {
        /*
          Quadratic Bézier curve.
        */
        do
        {
          points[0]=points[2];
          points[1].x=2.0*points[2].x-points[1].x;
          points[1].y=2.0*points[2].y-points[1].y;
          for (i=2; i < 3; i++)
          {
            (void) GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              (void) GetNextToken(p,&p,MagickPathExtent,token);
            x=GetDrawValue(token,&next_token);
            if (token == next_token)
              ThrowPointExpectedException(token,exception);
            (void) GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              (void) GetNextToken(p,&p,MagickPathExtent,token);
            y=GetDrawValue(token,&next_token);
            if (token == next_token)
              ThrowPointExpectedException(token,exception);
            end.x=(double) (attribute == (int) 'T' ? x : point.x+x);
            end.y=(double) (attribute == (int) 'T' ? y : point.y+y);
            points[i]=end;
          }
          if (status == MagickFalse)
            break;
          if (strchr("QqTt",last_attribute) == (char *) NULL)
            {
              points[0]=point;
              points[1]=point;
            }
          for (i=0; i < 3; i++)
            (q+i)->point=points[i];
          if (TraceBezier(mvg_info,3) == MagickFalse)
            return(-1);
          q=(*mvg_info->primitive_info)+mvg_info->offset;
          mvg_info->offset+=q->coordinates;
          q+=q->coordinates;
          point=end;
          last_attribute=attribute;
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 'v':
      case 'V':
      {
        /*
          Line to.
        */
        do
        {
          (void) GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            (void) GetNextToken(p,&p,MagickPathExtent,token);
          y=GetDrawValue(token,&next_token);
          if (token == next_token)
            ThrowPointExpectedException(token,exception);
          point.y=(double) (attribute == (int) 'V' ? y : point.y+y);
          if (CheckPrimitiveExtent(mvg_info,PrimitiveExtentPad) == MagickFalse)
            return(-1);
          q=(*mvg_info->primitive_info)+mvg_info->offset;
          if (TracePoint(q,point) == MagickFalse)
            return(-1);
          mvg_info->offset+=q->coordinates;
          q+=q->coordinates;
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 'z':
      case 'Z':
      {
        /*
          Close path.
        */
        point=start;
        if (CheckPrimitiveExtent(mvg_info,PrimitiveExtentPad) == MagickFalse)
          return(-1);
        q=(*mvg_info->primitive_info)+mvg_info->offset;
        if (TracePoint(q,point) == MagickFalse)
          return(-1);
        mvg_info->offset+=q->coordinates;
        q+=q->coordinates;
        primitive_info=(*mvg_info->primitive_info)+subpath_offset;
        primitive_info->coordinates=(size_t) (q-primitive_info);
        primitive_info->closed_subpath=MagickTrue;
        number_coordinates+=primitive_info->coordinates;
        primitive_info=q;
        subpath_offset=mvg_info->offset;
        z_count++;
        break;
      }
      default:
      {
        ThrowPointExpectedException(token,exception);
        break;
      }
    }
  }
  if (status == MagickFalse)
    return(-1);
  primitive_info=(*mvg_info->primitive_info)+subpath_offset;
  primitive_info->coordinates=(size_t) (q-primitive_info);
  number_coordinates+=primitive_info->coordinates;
  for (i=0; i < (ssize_t) number_coordinates; i++)
  {
    q--;
    q->primitive=primitive_type;
    if (z_count > 1)
      q->method=FillToBorderMethod;
  }
  q=primitive_info;
  return((ssize_t) number_coordinates);
}