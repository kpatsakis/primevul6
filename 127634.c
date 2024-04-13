char * SWFShape_dumpOutline(SWFShape s) 	 
{ 	 
	struct out o; 	 
	int i;
	int x = 0, y = 0;
 
	o.len = 0; 	 
	o.ptr = o.buf = (char *)malloc(1); 	 
	*o.ptr = 0; 	 
	  	 
	for (i = 0; i < s->nRecords; i++) 	 
	{
		ShapeRecord *record = s->records + i;
		switch(record->type)
		{
		case SHAPERECORD_STATECHANGE:
		{
			if(!record->record.stateChange->flags & SWF_SHAPE_MOVETOFLAG)
				continue;
			x = record->record.stateChange->moveToX;
			y = record->record.stateChange->moveToY;
			oprintf(&o, "moveto %d,%d\n", x, y);
			break;
		}
	  	case SHAPERECORD_LINETO:
		{
			x += record->record.lineTo->dx;
			y += record->record.lineTo->dy;
			oprintf(&o, "lineto %d,%d\n", x, y);
			break; 	 
		} 	 
		case SHAPERECORD_CURVETO: 	 
		{ 	 
			int controlX = record->record.curveTo->controlx;
			int controlY = record->record.curveTo->controly;
			int anchorX = record->record.curveTo->anchorx;
			int anchorY = record->record.curveTo->anchory;

			oprintf(&o, "curveto %d,%d %d,%d\n", 	 
				x+controlX, y+controlY, 	 
				x+controlX+anchorX, y+controlY+anchorY); 	 
	  	 
				x += controlX + anchorX; 	 
				y += controlY + anchorY;
			break; 
		}
		default: break;
		}
	} 	 
	  	 
	*o.ptr = 0; 	 
	return o.buf; 	 
}