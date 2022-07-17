#!/bin/sh
# Part of 

echo Needs to be updated with Apple dev ID info
exit 0

if [ -f ~/.apple-notarize-info ]; then
    #Load the stored signing info
    #This should have something like:
    # export NOTARIZE_EMAIL=dan@kulp.com
    # export NOTARIZE_PWD=...password for the apple notarize system...
    # export DEVELOPMENT_TEAM=U8ZT9RPC6S
    . ~/.apple-notarize-info
fi

pushd .
cd CodeBlocks.app/Contents/Resources/share/codeblocks/plugins
# libProfiler doesn't sign correctly, we'll just delete it for now
rm libProfiler.dylib
find . -name "*.dylib" -exec codesign --options=runtime --force --deep --verify --verbose --sign ${DEVELOPMENT_TEAM} {} \;
popd
codesign --options=runtime --force --deep --verify --verbose --sign ${DEVELOPMENT_TEAM} ./CodeBlocks.app
codesign -vvvv ./CodeBlocks.app

rm -rf CodeBlocks-build.dmg CodeBlocks.dmg
hdiutil create -size 192m -fs HFS+ -volname "CodeBlocks" CodeBlocks-build.dmg
hdiutil attach CodeBlocks-build.dmg
cp -a CodeBlocks.app /Volumes/CodeBlocks
ln -s /Applications /Volumes/CodeBlocks/Applications
DEVS=$(hdiutil attach CodeBlocks-build.dmg | cut -f 1)
DEV=$(echo $DEVS | head -n 1 | cut -f 1 -d ' ')
hdiutil detach $DEV
hdiutil convert CodeBlocks-build.dmg -format UDZO -o CodeBlocks.dmg

xcrun altool --notarize-app -f CodeBlocks.dmg --primary-bundle-id org.codeblocks.app  -u ${NOTARIZE_EMAIL} -p @env:NOTARIZE_PWD

echo "Run xcrun altool --notarization-info UUID -u ${NOTARIZE_EMAIL} -p @env:NOTARIZE_PWD"
read -p "Press any key to continue... " -n1 -s

xcrun stapler staple -v CodeBlocks.app

#now its signed/notarized, repackage
rm -rf CodeBlocks-build.dmg CodeBlocks.dmg
hdiutil create -size 192m -fs HFS+ -volname "CodeBlocks" CodeBlocks-build.dmg
hdiutil attach CodeBlocks-build.dmg
cp -a CodeBlocks.app /Volumes/CodeBlocks
ln -s /Applications /Volumes/CodeBlocks/Applications
DEVS=$(hdiutil attach CodeBlocks-build.dmg | cut -f 1)
DEV=$(echo $DEVS | head -n 1 | cut -f 1 -d ' ')
hdiutil detach $DEV
hdiutil convert CodeBlocks-build.dmg -format UDZO -o CodeBlocks.dmg

codesign --force --sign ${DEVELOPMENT_TEAM} CodeBlocks.dmg
spctl -a -t open --context context:primary-signature -v CodeBlocks.dmg

xcrun altool --notarize-app -f CodeBlocks.dmg  --primary-bundle-id org.codeblocks.app -u ${NOTARIZE_EMAIL} -p @env:NOTARIZE_PWD
echo "Run xcrun altool --notarization-info UUID -u ${NOTARIZE_EMAIL} -p @env:NOTARIZE_PWD"
read -p "Press any key to continue... " -n1 -s

# staple the DMG's notarization to the dmg
xcrun stapler staple -v CodeBlocks.dmg
spctl -a -t open --context context:primary-signature -v CodeBlocks.dmg
