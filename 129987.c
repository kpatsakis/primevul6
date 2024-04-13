alloc_table (MonoDynamicTable *table, guint nrows)
{
	table->rows = nrows;
	g_assert (table->columns);
	if (nrows + 1 >= table->alloc_rows) {
		while (nrows + 1 >= table->alloc_rows) {
			if (table->alloc_rows == 0)
				table->alloc_rows = 16;
			else
				table->alloc_rows *= 2;
		}

		table->values = g_renew (guint32, table->values, (table->alloc_rows) * table->columns);
	}
}