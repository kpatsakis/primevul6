static void lookforheader(aac_buffer *b)
{
  int i = 0;
	while (!b->at_eof )
	{
       		if (b->bytes_into_buffer > 4)
        	{
		    if( ((b->buffer[0+i] == 0xff) && ((b->buffer[1+i] & 0xf6) == 0xf0)) ||
       			 (b->buffer[0+i] == 'A'    && b->buffer[1+i] == 'D' && b->buffer[2+i] == 'I' && b->buffer[3+i] == 'F'))
			{
				fill_buffer(b);
				break;
			} else {
				i++;
    				b->file_offset       += 1;
    				b->bytes_consumed    += 1;
    				b->bytes_into_buffer -= 1;
			}
		} else {
			fill_buffer(b);
			i = 0;
		}
	}
}