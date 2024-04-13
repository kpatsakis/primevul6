static QuantizationTable *DestroyQuantizationTable(QuantizationTable *table)
{
  assert(table != (QuantizationTable *) NULL);
  if (table->slot != (char *) NULL)
    table->slot=DestroyString(table->slot);
  if (table->description != (char *) NULL)
    table->description=DestroyString(table->description);
  if (table->levels != (unsigned int *) NULL)
    table->levels=(unsigned int *) RelinquishMagickMemory(table->levels);
  table=(QuantizationTable *) RelinquishMagickMemory(table);
  return(table);
}