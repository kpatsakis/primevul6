QPDFWriter::unparseChild(QPDFObjectHandle child, int level, int flags)
{
    if (! this->linearized)
    {
	enqueueObject(child);
    }
    if (child.isIndirect())
    {
	QPDFObjGen old_og = child.getObjGen();
	int new_id = obj_renumber[old_og];
	writeString(QUtil::int_to_string(new_id));
	writeString(" 0 R");
    }
    else
    {
	unparseObject(child, level, flags);
    }
}