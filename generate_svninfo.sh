#!/bin/sh

URL_title="URL:"
WORKDIR_title="Working Copy Root Path:"
SRCREV_title="Revision:"
LAST_CHANGER_title="Last Changed Author:"
LASTREV_title="Last Changed Rev:"
LASTMODATE_title="Last Changed Date:"
SVNINFO_FILE_NAME="svn_info.c"


svn_url=`svn info|grep "$URL_title"|sed "s/$URL_title//"`
work_dir=`svn info|grep "$WORKDIR_title"|sed "s/$WORKDIR_title//"`
src_revision=`svn info|grep "$SRCREV_title"|sed "s/$SRCREV_title//"`
src_last_change_author=`svn info|grep "$LAST_CHANGER_title"|sed "s/$LAST_CHANGER_title//"`
src_last_change_revision=`svn info|grep "$LASTREV_title"|sed "s/$LASTREV_title//"`
src_last_change_date=`svn info|grep "$LASTMODATE_title"|sed "s/$LASTMODATE_title//"`

echo '#include "svn_info.h"'>$SVNINFO_FILE_NAME
echo "DECLARE_VERSION_INFO(\"$src_last_change_author\",\"$svn_url\",\"$work_dir\",\"$src_revision\",\"$src_last_change_revision\",\"$src_last_change_date\")">>$SVNINFO_FILE_NAME




