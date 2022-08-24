#!/bin/bash

if [ "${MAC_OSX_MIN_VERSION}" == "" ] ; then
    MAC_OSX_MIN_VERSION=11.6
fi
WXVERSION=wx`wx-config --version`
SRC_FOLDER="./CodeBlocks.app"

if [ ! -d ${SRC_FOLDER} ]; then
	echo "+----------------------------------------------------------------------------+"
	echo "|                                                                            |"
	echo "| Sorry, but the directory below does not exist. Please fix and try again!!! |" 
	echo "|     Missing Directory: ${SRC_FOLDER}                                   |"
	echo "|                                                                            |"
	echo "+----------------------------------------------------------------------------+"
    exit 1
fi

pushd ..
SF_CB_SVN_REPO=https://svn.code.sf.net/p/codeblocks/code/trunk
if svn --xml info ${SF_CB_SVN_REPO} >/dev/null 2>&1; then
	echo "Using 'svn --xml info ${SF_CB_SVN_REPO}' to get the revision"
	SVN_REV=`svn --xml info ${SF_CB_SVN_REPO} | tr -d '\r\n' | sed -e 's/.*<commit.*revision="\([0-9]*\)".*<\/commit>.*/\1/'`
	SVN_DATE=`svn --xml info ${SF_CB_SVN_REPO} | tr -d '\-\r\n' | sed -e 's/.*<commit.*<date>\([0-9\-]*\)\T\([0-9\:]*\)\..*<\/date>.*<\/commit>.*/\1/'`
elif svn --info ${SF_CB_SVN_REPO} >/dev/null 2>&1; then
	echo "Using 'svn info ${SF_CB_SVN_REPO}' to get the revision"
	SVN_REV=`svn info ${SF_CB_SVN_REPO} | grep "^Revision:" | cut -d" " -f2`
	SVN_DATE=`svn info ${SF_CB_SVN_REPO} | grep "^Last Changed Date:" | cut -d" " -f4,5`
#elif git svn --version >/dev/null 2>&1; then
#	echo "Using 'git svn info' to get the revision"
#	SVN_REV=`git svn info | grep "^Revision:" | cut -d" " -f2`
#	LCD=`git svn info | grep "^Last Changed Date:" | cut -d" " -f4,5`
elif git log --max-count=1 >/dev/null 2>&1; then
	echo "Using 'git log --graph' to get the revision"
	SVN_REV=`git log --graph | grep 'git-svn-id' | head -n 1 | grep -o -e "@\([0-9]*\)" | tr -d '@ '`
	SVN_DATE=`git log --date=iso --max-count=1 | grep -o -e "Date: \(.*\)" | cut -d ' ' -f 2- | sed 's/^ *//' | cut -f -2 -d ' '`
else
	SVN_REV=0
	SVN_DATE=""
fi
popd

#echo "SVN_REV=${SVN_REV}"
#echo "SVN_DATE=${SVN_DATE}"

VOLUME_NAME="CB_${SVN_DATE}_rev${SVN_REV}_macOS-${MAC_OSX_MIN_VERSION}_x64"
DMG_NAME="${VOLUME_NAME}-${WXVERSION}.dmg"
DMG_TMP_NAME="${VOLUME_NAME}-tmp.dmg"
VOLUME_SIZE=200m

echo "Creating DMG file '${DMG_NAME}'"
#echo "Running: hdiutil create -srcfolder \"${SRC_FOLDER}\" -volname \"${VOLUME_NAME}\" -fs HFS+ -fsargs \"-c c=64,a=16,e=16\" -format UDRW -size ${VOLUME_SIZE} ${DMG_TMP_NAME}"
hdiutil create -srcfolder "${SRC_FOLDER}" \
                -volname "${VOLUME_NAME}" \
                -fs HFS+ \
                -fsargs "-c c=64,a=16,e=16" \
                -format UDRW \
                -size ${VOLUME_SIZE} \
                ${DMG_TMP_NAME}

echo "Mouting DMG file"
DEVICE=$(hdiutil attach -readwrite -noverify -noautoopen ${DMG_TMP_NAME} | \
                egrep '^/dev/' | sed 1q | awk '{print $1}')

echo "Adding background image and Applications link"
mkdir "/Volumes/${VOLUME_NAME}/.background"

cp CB_dmg-background.png "/Volumes/${VOLUME_NAME}/.background/codeblocks.png"

ln -s /Applications "/Volumes/${VOLUME_NAME}/Applications"

echo "Copying volume icon file..."
cp "./src/src/resources/icons/app.icns" "/Volumes/${VOLUME_NAME}/.VolumeIcon.icns"
SetFile -c icnC "/Volumes/${VOLUME_NAME}/.VolumeIcon.icns"

echo "Customizing disk image"

echo '
	tell application "Finder"
		tell disk "'${VOLUME_NAME}'"
			open
			
			set theXOrigin to 50
			set theYOrigin to 50
			set theWidth to 500
			set theHeight to 400

            set theBottomRightX to (theXOrigin + theWidth)
			set theBottomRightY to (theYOrigin + theHeight)
			set dsStore to "/Volumes/'${VOLUME_NAME}'/.DS_STORE"

            tell container window
				set current view to icon view
				set toolbar visible to false
				set statusbar visible to false
				set the bounds to {theXOrigin, theYOrigin, theBottomRightX, theBottomRightY}
				set statusbar visible to false
				set position of every item to (theBottomRightX + 100, 100)
			end tell

            set opts to the icon view options of container window
			tell opts
				set icon size to 128
				set text size to 16
				set arrangement to not arranged
			end tell
            set background picture of opts to file ".background:codeblocks.png"
            
            set position of item "CodeBlocks.app" to (20,20)
            -- the following does not works (perhaps because it is a folder)
            set the extension hidden of item "CodeBlocks.app" to true
			
			set position of item "Applications" to (300,20)
            close
            open
            update without registering applications
			delay 1

            tell container window
				set statusbar visible to false
				set the bounds to {theXOrigin, theYOrigin, theBottomRightX - 10, theBottomRightY - 10}
			end tell

            update without registering applications

        end tell

        delay 1
		
		tell disk "'${VOLUME_NAME}'"
			tell container window
				set statusbar visible to false
				set the bounds to {theXOrigin, theYOrigin, theBottomRightX, theBottomRightY}
			end tell
			
			update without registering applications
		end tell

        --give the finder some time to write the .DS_Store file
		delay 3

        set waitTime to 0
		set ejectMe to false
		repeat while ejectMe is false
			delay 1
			set waitTime to waitTime + 1
			
			if (do shell script "[ -f " & dsStore & " ]; echo $?") = "0" then set ejectMe to true
		end repeat
		log "Waited " & waitTime & " seconds for .DS_STORE to be created."
	end tell
' | osascript

echo "Waiting a few seconds..."
sleep 5

echo "Fixing permissions"
chmod -Rf go-w "/Volumes/${VOLUME_NAME}" &> /dev/null || true

echo "Blessing"
bless --folder "/Volumes/${VOLUME_NAME}" --openfolder "/Volumes/${VOLUME_NAME}"

echo "Unmouting disk image"
hdiutil detach ${DEVICE}

echo "Compressing disk image..."
hdiutil convert "${DMG_TMP_NAME}" -format UDZO -imagekey zlib-level=9 -o "${DMG_NAME}"
rm -f "${DMG_TMP_NAME}"

if [ -d "/Volumes/${VOLUME_NAME}" ]; then
    hdiutil detach "/Volumes/${VOLUME_NAME}"
fi

echo "DMG disk image created."

