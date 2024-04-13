CompositeDeepScanLine::sources() const
{
   return int(_Data->_part.size())+int(_Data->_file.size());
}