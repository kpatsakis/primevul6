QPDFWriter::writeBuffer(PointerHolder<Buffer>& b)
{
    this->pipeline->write(b->getBuffer(), b->getSize());
}