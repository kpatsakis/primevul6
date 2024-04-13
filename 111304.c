CompositeDeepScanLine::addSource(DeepScanLineInputPart* part)
{
  _Data->check_valid(part->header());
  _Data->_part.push_back(part);
}