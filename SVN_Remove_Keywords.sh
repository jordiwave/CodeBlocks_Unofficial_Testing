#!/bin/bash
# Script that combines Tickets 1163 and 1172 changes

if [ "$(id -u)" == "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

for srcFilename in $(grep -rl --include \*.cpp --include \*.h --exclude .svn\* '* $Revision: ' ./)
do 
    echo "sed - $srcFilename" >> sed_filelist.txt
    sed -i '/\* \$Revision\:/d' $srcFilename;
    sed -i '/\* \$Id\:/d' $srcFilename;
    sed -i '/\* \$HeadURL\:/d' $srcFilename;
done

for srcFilename in $(grep -rl --include \manifest.xml --include \*.script --exclude .svn\* '* $Revision: ' ./)
do 
    echo "sed - $srcFilename" >> sed_filelist.txt
    sed -i '/\* \$Revision\:/d' $srcFilename;
    sed -i '/\* \$Id\:/d' $srcFilename;
    sed -i '/\* \$HeadURL\:/d' $srcFilename;
done

for srcFilename in $(grep -rl --include \*.sh --include \*.ac --include \*.m4 --exclude SVN_Remove_Keywords.sh --exclude .svn\* '* $Revision: ' ./)
do 
    echo "sed - $srcFilename" >> sed_filelist.txt
    sed -i '/\* \$Revision\:/d' $srcFilename;
    sed -i '/\* \$Id\:/d' $srcFilename;
    sed -i '/\* \$HeadURL\:/d' $srcFilename;
done


for srcFilename in $(grep -l '# $Revision: ' update*.sh )
do 
    echo "sed - $srcFilename" >> sed_filelist.txt
    sed -i '/\# \$Revision\:/d' $srcFilename;
    sed -i '/\# \$Id\:/d' $srcFilename;
    sed -i '/\# \$HeadURL\:/d' $srcFilename;
done

for srcFilename in $(grep -rl --include '\update*' --exclude .svn\* '# $Revision: ' ./)
do 
    echo "sed - $srcFilename" >> sed_filelist.txt
    sed -i '/\# \$Revision\:/d' $srcFilename;
    sed -i '/\# \$Id\:/d' $srcFilename;
    sed -i '/\# \$HeadURL\:/d' $srcFilename;
done

for srcFilename in $(grep -rl --include '\update*.bat' --exclude .svn\* 'rem $Revision: ' ./)
do 
    echo "sed - $srcFilename" >> sed_filelist.txt
    sed -i '/rem \$Revision\:/d' $srcFilename;
    sed -i '/rem \$Id\:/d' $srcFilename;
    sed -i '/rem \$HeadURL\:/d' $srcFilename;
done

# ================================= RCS stuff below =================================
for srcFilename in $(grep -rl --include \*.cpp --include \*.h RCS-ID ./)
do 
    echo "sed - $srcFilename"
    sed -i '/\/\/\s\+RCS-ID\:/d' $srcFilename;
done

for srcFilename in $(grep -rl --include \*.sh --include \*.ac --include \*.m4 --exclude SVN_Remove_Keywords.sh RCS-ID ./)
do 
    echo "sed - $srcFilename"
    sed -i '/\#\s\+RCS-ID\:/d' $srcFilename;
done
