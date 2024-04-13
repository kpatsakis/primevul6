SWFShape_writeShapeRecord(SWFShape shape, ShapeRecord record, SWFOutput out)
{
	switch(record.type)
	{
		case SHAPERECORD_STATECHANGE:
		{
			int flags = record.record.stateChange->flags;

			if(flags == 0)
				return;

			SWFOutput_writeBits(out, flags, 6);

			if(flags & SWF_SHAPE_MOVETOFLAG)
			{
				int x = record.record.stateChange->moveToX;
				int y = record.record.stateChange->moveToY;
				int nBits = max(SWFOutput_numSBits(x), SWFOutput_numSBits(y));

				SWF_assert(nBits<32);
				SWFOutput_writeBits(out, nBits, 5);
				SWFOutput_writeSBits(out, x, nBits);
				SWFOutput_writeSBits(out, y, nBits);
			}

			if(flags & SWF_SHAPE_FILLSTYLE0FLAG)
			{
				SWFOutput_writeBits(out, record.record.stateChange->leftFill,
														SWFOutput_numBits(shape->nFills));
			}

			if(flags & SWF_SHAPE_FILLSTYLE1FLAG)
			{
				SWFOutput_writeBits(out, record.record.stateChange->rightFill,
														SWFOutput_numBits(shape->nFills));
			}

			if(flags & SWF_SHAPE_LINESTYLEFLAG)
			{
				SWFOutput_writeBits(out, record.record.stateChange->line,
														SWFOutput_numBits(shape->nLines));
			}

			/* newstyle's never used.	 But this is what it looks like:

			if ( flags & SWF_SHAPE_NEWSTYLEFLAG )
			{
				SWFOutput_writeFillStyles(shape->out, shape->fills, shape->nFills,
				BLOCK(shape)->type);

				SWFOutput_writeLineStyles(shape->out, shape->lines, shape->nLines,
					BLOCK(shape)->type);

				SWFOutput_writeBits(shape->out, SWFOutput_numBits(shape->nFills), 4);
				SWFOutput_writeBits(shape->out, SWFOutput_numBits(shape->nLines), 4);
			}

			*/

			break;
		}

		case SHAPERECORD_LINETO:
		{
			int nBits;
			int dx = record.record.lineTo->dx;
			int dy = record.record.lineTo->dy;

			SWFOutput_writeBits(out, 3, 2); /* straight edge */

			if(dx==0)
			{
				nBits = SWFOutput_numSBits(dy);
				SWF_assert(nBits<18);
				SWFOutput_writeBits(out, nBits-2, 4);
				SWFOutput_writeBits(out, 1, 2); /* vertical line */
				SWFOutput_writeSBits(out, dy, nBits);
			}
			else if(dy==0)
			{
				nBits = SWFOutput_numSBits(dx);
				SWF_assert(nBits<18);
				SWFOutput_writeBits(out, nBits-2, 4);
				SWFOutput_writeBits(out, 0, 2); /* horizontal line */
				SWFOutput_writeSBits(out, dx, nBits);
			}
			else
			{
				nBits = max(SWFOutput_numSBits(dx), SWFOutput_numSBits(dy));
				SWF_assert(nBits<18);
				SWFOutput_writeBits(out, nBits-2, 4);
				SWFOutput_writeBits(out, 1, 1); /* general line */
				SWFOutput_writeSBits(out, dx, nBits);
				SWFOutput_writeSBits(out, dy, nBits);
			}

			break;
		}

		case SHAPERECORD_CURVETO:
		{
			int controlx = record.record.curveTo->controlx;
			int controly = record.record.curveTo->controly;
			int anchorx = record.record.curveTo->anchorx;
			int anchory = record.record.curveTo->anchory;

			int nBits = max(max(SWFOutput_numSBits(controlx),
													SWFOutput_numSBits(controly)),
											max(SWFOutput_numSBits(anchorx),
													SWFOutput_numSBits(anchory)));

			if ( nBits < 2 )
				nBits = 2;

			SWF_assert(nBits < 18);

			SWFOutput_writeBits(out, 2, 2); /* curved edge */
			SWFOutput_writeBits(out, nBits-2, 4);
			SWFOutput_writeSBits(out, controlx, nBits);
			SWFOutput_writeSBits(out, controly, nBits);
			SWFOutput_writeSBits(out, anchorx, nBits);
			SWFOutput_writeSBits(out, anchory, nBits);

			break;
		}

		default:
			SWF_error("Unknown shapeRecordType");
	}
}