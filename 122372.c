static void sixel_advance(sixel_output_t *context, int nwrite)
{
    if ((context->pos += nwrite) >= SIXEL_OUTPUT_PACKET_SIZE) {
        WriteBlob(context->image,SIXEL_OUTPUT_PACKET_SIZE,context->buffer);
        CopyMagickMemory(context->buffer,
               context->buffer + SIXEL_OUTPUT_PACKET_SIZE,
               (context->pos -= SIXEL_OUTPUT_PACKET_SIZE));
    }
}