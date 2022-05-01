#!/bin/bash

# --------------------------------------------------------------------------------------#
#                                                                                       #
# This file is part of the Code::Blocks IDE and licensed under the                      #
#  GNU General Public License, version 3. http://www.gnu.org/licenses/gpl-3.0.html      #
#                                                                                       #
# --------------------------------------------------------------------------------------#

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

# -----------------------------------------------------------------------------

# make sure [git-]svn answers in english
export LC_ALL="C"

REV_FILE=./revision.m4
SF_CB_SVN_REPO=https://svn.code.sf.net/p/codeblocks/code/trunk

if svn --xml info $SF_CB_SVN_REPO >/dev/null 2>&1; then
	echo "Using 'svn --xml info $SF_CB_SVN_REPO' to get the revision"
	REV=`svn --xml info $SF_CB_SVN_REPO | tr -d '\r\n' | sed -e 's/.*<commit.*revision="\([0-9]*\)".*<\/commit>.*/\1/'`
	LCD=`svn --xml info $SF_CB_SVN_REPO | tr -d '\r\n' | sed -e 's/.*<commit.*<date>\([0-9\-]*\)\T\([0-9\:]*\)\..*<\/date>.*<\/commit>.*/\1 \2/'`
elif svn --info $SF_CB_SVN_REPO >/dev/null 2>&1; then
	echo "Using 'svn info $SF_CB_SVN_REPO' to get the revision"
	REV=`svn info $SF_CB_SVN_REPO | grep "^Revision:" | cut -d" " -f2`
	LCD=`svn info $SF_CB_SVN_REPO | grep "^Last Changed Date:" | cut -d" " -f4,5`
#elif git svn --version >/dev/null 2>&1; then
#	echo "Using 'git svn info' to get the revision"
#	REV=`git svn info | grep "^Revision:" | cut -d" " -f2`
#	LCD=`git svn info | grep "^Last Changed Date:" | cut -d" " -f4,5`
elif git log --max-count=1 >/dev/null 2>&1; then
	echo "Using 'git log --graph' to get the revision"
	REV=`git log --graph | grep 'git-svn-id' | head -n 1 | grep -o -e "@\([0-9]*\)" | tr -d '@ '`
	LCD=`git log --date=iso --max-count=1 | grep -o -e "Date: \(.*\)" | cut -d ' ' -f 2- | sed 's/^ *//' | cut -f -2 -d ' '`
else
	REV=0
	LCD=""
fi

echo "Found revision: '${REV}' '${LCD}'"

if [ "x$REV" != "x$0" -o ! -r $REV_FILE ]; then
	CURRENTDATE=$(date +"%Y.%m.%d")
	echo "m4_define([SVN_REV], ${REV})" > $REV_FILE
	echo "m4_define([SVN_REVISION], ${CURRENTDATE}svn${REV})" >> $REV_FILE
	echo "m4_define([SVN_DATE], ${LCD})" >> $REV_FILE
fi

exit 0
