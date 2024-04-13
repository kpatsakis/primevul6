KCleanup::expandVariables( const KFileInfo *	item,
			   const QString &	unexpanded ) const
{
    QString expanded = unexpanded;

    expanded.replace( QRegExp( "%p" ),
		      "'" + QString::fromLocal8Bit( item->url() )  + "'" );
    expanded.replace( QRegExp( "%n" ),
		      "'" + QString::fromLocal8Bit( item->name() ) + "'" );

   // if ( KDE::versionMajor() >= 3 && KDE::versionMinor() >= 4 )
	expanded.replace( QRegExp( "%t" ), "trash:/" );
    //else
	//expanded.replace( QRegExp( "%t" ), KGlobalSettings::trashPath() );

    return expanded;
}