#!/bin/bash

# --------------------------------------------------------------------------------------#
#                                                                                       #
# This file is part of the Code::Blocks IDE and licensed under the                      #
#  GNU General Public License, version 3. http://www.gnu.org/licenses/gpl-3.0.html      #
#                                                                                       #
# --------------------------------------------------------------------------------------#
#                                                                                       #
#    This bash script updates the CodeBlocks devel3x_64 directory on:                   #
#           - Windows using MSYS2 Mingw64                                               #
#           - Linux                                                                     #
#           - MACOS
#                                                                                       #
# --------------------------------------------------------------------------------------#

if [ "$(id -u)" eq "0" ]; then
    echo "You are root. Please run again as a normal user!!!"
    exit 1
fi

# ----------------------------------------------------------------------------
# Set build variables
# ----------------------------------------------------------------------------
InitialDir=${PWD}

# -------------------------------------------------------------------------------------------------

if [ "${GITHUB_ACTIONS}" != "true" ] ; then
    # The following is to enable sending the output of this script to the terminal and to the 
    # file specified:
    exec > >(tee -i codeBlocks_Update_Dev.log) 2>&1
    # NOTE: if you want to append to the file change the -i to -ia in the line above.
fi

# -------------------------------------------------------------------------------------------------

case "$(uname)" in
  Darwin*)
    OSDetected="OSX"
    EXEEXT=""
    LIBEXT="dylib"
    ;;
  Linux*)
    OSDetected="Linux"
    EXEEXT=""
    LIBEXT="so"
    ;;
  MINGW* | msys* | cygwin* | WindowsNT)
    OSDetected="Windows"
    EXEEXT=".exe"
    LIBEXT="dll" 
    ;;
  AIX*)
    echo "AIX is not supported"
    cd ${InitialDir}
    exit 0
    ;;
  bsd*)
    echo "BSD is not supported"
    cd ${InitialDir}
    exit 0
    ;;
  bsd*)
    echo "BSD is not supported"
    cd ${InitialDir}
    exit 0
    ;;
  FreeBSD*)
    echo "FreeBSD is not supported"
    cd ${InitialDir}
    exit 0
    ;;
  solaris*)
    echo "SOLARIS is not supported"
    cd ${InitialDir}
    exit 0
    ;;
  SunOS*)
    echo "SunOS is not supported"
    cd ${InitialDir}
    exit 0
    ;;
  *)
    echo "Unknown: ${OSTYPE} is not supported"
    cd ${InitialDir}
    exit 0
    ;;
esac

# -----------------------------------------------------------------------------

reset
echo
echo "+-------------------------------------------------------------------------------------------------+"
echo "|                                                                                                 |"
echo "|                           Updating C::B directory build files.                                  |"
echo "|                                                                                                 |"
echo "|    Detected OS:                        ${OSDetected}                                            |"
echo "|    uname:                              $(uname)                                            |"

# ----------------------------------------------------------------------------
# Check where we are running from and go to the C::B source root directory 
# ----------------------------------------------------------------------------

if [ ! -f "bootstrap" ]; then
    if [ -f "../bootstrap" ]; then
        cd ..
    else
        if [ -f "../../bootstrap" ]; then
            cd ../..
        else
            echo Could not find bootstrap or ../bootstrap or ../../bootstrap
            cd ${InitialDir}
            exit 3
        fi
    fi
fi

echo "|    CurrentDir C::B root is:            ${PWD}                                                  |"

CB_ROOT=$PWD
CB_SRC=${CB_ROOT}/src

# ----------------------------------------------------------------------------
# Check BUILD_BITS for validity
# ----------------------------------------------------------------------------

DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC}/devel30 2>/dev/null | wc -l 2>/dev/null)
if [ ${DEVEL_DIR_COUNT} -gt 0 ] ; then  BUILD_BITS=64 ; CB_DEVEL_DIR=${CB_SRC}/devel30 ; fi

DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC}/devel30_32 2>/dev/null | wc -l 2>/dev/null)
if [ ${DEVEL_DIR_COUNT} -gt 0 ] ; then  BUILD_BITS=32 ; CB_DEVEL_DIR=${CB_SRC}/devel30_32 ; fi

DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC}/devel30_64 2>/dev/null | wc -l 2>/dev/null)
if [ ${DEVEL_DIR_COUNT} -gt 0 ] ; then  BUILD_BITS=64 ; CB_DEVEL_DIR=${CB_SRC}/devel30_64 ; fi

# -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC}/devel31 2>/dev/null | wc -l 2>/dev/null)
if [ ${DEVEL_DIR_COUNT} -gt 0 ] ; then  BUILD_BITS=64 ; CB_DEVEL_DIR=${CB_SRC}/devel31 ; fi

DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC}/devel31_32 2>/dev/null | wc -l 2>/dev/null)
if [ ${DEVEL_DIR_COUNT} -gt 0 ] ; then  BUILD_BITS=32 ; CB_DEVEL_DIR=${CB_SRC}/devel31_32 ; fi

DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC}/devel31_64 2>/dev/null | wc -l 2>/dev/null)
if [ ${DEVEL_DIR_COUNT} -gt 0 ] ; then  BUILD_BITS=64 ; CB_DEVEL_DIR=${CB_SRC}/devel31_64 ; fi

# -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC}/devel32 2>/dev/null | wc -l 2>/dev/null)
if [ ${DEVEL_DIR_COUNT} -gt 0 ] ; then  BUILD_BITS=64 ; CB_DEVEL_DIR=${CB_SRC}/devel32 ; fi

DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC}/devel32_32 2>/dev/null | wc -l 2>/dev/null)
if [ ${DEVEL_DIR_COUNT} -gt 0 ] ; then  BUILD_BITS=32 ; CB_DEVEL_DIR=${CB_SRC}/devel32_32 ; fi

DEVEL_DIR_COUNT=$(ls -1q ${CB_SRC}/devel32_64 2>/dev/null | wc -l 2>/dev/null)
if [ ${DEVEL_DIR_COUNT} -gt 0 ] ; then  BUILD_BITS=64 ; CB_DEVEL_DIR=${CB_SRC}/devel32_64 ; fi

# -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

if [ "${CB_DEVEL_DIR}" == "" ] ; then
    echo "+-------------------------------------------------------------------------------------------------+"
    echo "|                                                                                                 |"
    echo "|        +-------------------------------------------------------------+                          |"
    echo "|        | Error: Cannot find src\devel3x  directory, where x in 0,1,2 |                          |"
    echo "|        |        Please fix and try again                             |                          |"
    echo "|        +-------------------------------------------------------------+                          |"
    echo "|                                                                                                 |"
    echo "+-------------------------------------------------------------------------------------------------+"
    echo
    echo BUILD_BITS:${BUILD_BITS}
    cd ${InitialDir}
    exit 4
fi

# -----------------------------------------------------------------------------

echo
echo "+-------------------------------------------------------------------------------------------------+"
echo "|                                                                                                 |"
echo "|                           Updating C::B directory build files.                                  |"
echo "|                                                                                                 |"
echo "| Detected OS:   ${OSDetected}                                                                          |"
echo "| Devel dir:     ${CB_DEVEL_DIR} |"
echo "| CB_ROOT:       ${CB_ROOT}        |"
echo "| CB_SRC:        ${CB_SRC}    |"
echo "| BUILD_BITS:    ${BUILD_BITS}                                                                               |"
echo "| PWD:           ${PWD}        |"
echo "+-------------------------------------------------------------------------------------------------+"

# ----------------------------------------------------------------------------
# Check if build succeeded
# ----------------------------------------------------------------------------
#case "$(OSDetected)" in
#  Windows*)
if { [ -d "${CB_DEVEL_DIR}/bin" ] && [ -d "${CB_DEVEL_DIR}/lib" ] 
} then
    if { !  {
                [ -f "${CB_DEVEL_DIR}/bin/codeblocks${EXEEXT}" ]                        &&
                [ -f "${CB_DEVEL_DIR}/lib/libcodeblocks.${LIBEXT}" ]                    &&
                {
                    [ -f "${CB_DEVEL_DIR}/lib/codeblocks/plugins/todo.${LIBEXT}" ] ||
                    [ -f "${CB_DEVEL_DIR}/lib/codeblocks/plugins/libtodo.${LIBEXT}" ]
                }
            }
       } then
        echo "|                                                                                                 |"
        echo "|             +--------------------------------------------------------------------+              |"
        echo "|             | Error: Code::Blocks make error was detected.                       |              |"
        echo "|             |        Please fix the error and try again.                         |              |"
        [ ! -f "${CB_DEVEL_DIR}/bin/codeblocks${EXEEXT}" ]                     && echo "|             |        Missing ${CB_DEVEL_DIR}/bin/codeblocks${EXEEXT}!                          |              |"
        [ ! -f "${CB_DEVEL_DIR}/lib/libcodeblocks.${LIBEXT}" ]                 && echo "|             |        Missing ${CB_DEVEL_DIR}/lib/libcodeblocks.${LIBEXT}!                 |              |"
        [ ! -f "${CB_DEVEL_DIR}/lib/codeblocks/plugins/todo.${LIBEXT}" ]       && echo "|             |        Missing ${CB_DEVEL_DIR}/lib/codeblocks/plugins/todo.${LIBEXT}! |              |"
        [ ! -f "${CB_DEVEL_DIR}/lib/codeblocks/plugins/libtodo.${LIBEXT}" ]    && echo "|             |        Missing ${CB_DEVEL_DIR}/lib/codeblocks/plugins/libtodo.${LIBEXT}! |              |"
        echo "|             +--------------------------------------------------------------------+              |"
        echo "|                                                                                                 |"
        echo "+-------------------------------------------------------------------------------------------------+"
        echo
        cd ${InitialDir}
        exit 5
    fi
else
    if { !  {
                [ -f "${CB_DEVEL_DIR}/codeblocks${EXEEXT}" ]                        &&
                [ -f "${CB_DEVEL_DIR}/libcodeblocks.${LIBEXT}" ]                    &&
                {
                    [ -f "${CB_DEVEL_DIR}/share/codeblocks/plugins/todo.${LIBEXT}" ] ||
                    [ -f "${CB_DEVEL_DIR}/share/codeblocks/plugins/libtodo.${LIBEXT}" ]
                }
            }
       } then
        echo "|                                                                                                 |"
        echo "|             +--------------------------------------------------------------------+              |"
        echo "|             | Error: Code::Blocks make error was detected.                       |              |"
        echo "|             |        Please fix the error and try again.                         |              |"
        [ ! -f "${CB_DEVEL_DIR}/codeblocks${EXEEXT}" ]                     && echo "|             |        Missing ${CB_DEVEL_DIR}/codeblocks${EXEEXT}!                          |              |"
        [ ! -f "${CB_DEVEL_DIR}/libcodeblocks.${LIBEXT}" ]                 && echo "|             |        Missing ${CB_DEVEL_DIR}/libcodeblocks.${LIBEXT}!                 |              |"
        [ ! -f "${CB_DEVEL_DIR}/share/codeblocks/plugins/todo.${LIBEXT}" ] && echo "|             |        Missing ${CB_DEVEL_DIR}/share/codeblocks/plugins/todo.${LIBEXT}! |              |"
        [ ! -f "${CB_DEVEL_DIR}/share/codeblocks/plugins/libtodo.${LIBEXT}" ] && echo "|             |        Missing ${CB_DEVEL_DIR}/share/codeblocks/plugins/libtodo.${LIBEXT}! |              |"
        echo "|             +--------------------------------------------------------------------+              |"
        echo "|                                                                                                 |"
        echo "+-------------------------------------------------------------------------------------------------+"
        echo
        cd ${InitialDir}
        exit 5
    fi
fi

# ----------------------------------------------------------------------------
# Detect GCC directory 
# ----------------------------------------------------------------------------
GCC_ROOT=$(dirname $(which g++))
if [ ! -d "${GCC_ROOT}" ]; then
    echo "|                                                                                                 |"
    echo "|                                    +-----------------------------+                              |"
    echo "|                                    |     Error: NO GCC found     |                              |"
    echo "|                                    +-----------------------------+                              |"
    echo "|                                                                                                 |"
    cd ${InitialDir}
    exit 6
fi
echo "|    GCC_ROOT root detected at:          ${GCC_ROOT}                                             |"


# ----------------------------------------------------------------------------
# Detect wxWidget directory 
# ----------------------------------------------------------------------------
if [ "${OSDetected}" = "Windows" ] ; then
    if [ ! -d "$WX_CB_BUILD_DIR" ]; then
        echo "|                                                                                                 |"
        echo "|             +---------------------------------------------------------+                         |"
        echo "|             |  Error: NO WX_CB_BUILD_DIR environment variable set     |                         |"
        echo "|             |           Please export WX_CB_BUILD_DIR environment and |                         |"
        echo "|             |           try again.                                    |                         |"
        echo "|             |           WX_CB_BUILD_DIR is wxWidget CB build src      |                         |"
        echo "|             |           root directory.                               |                         |"
        echo "|             +---------------------------------------------------------+                         |"
        echo "|                                                                                                 |"
        echo "+-------------------------------------------------------------------------------------------------+"
        echo
        cd ${InitialDir}
        exit 7
    fi
    echo "|    WX_CB_BUILD_DIR set to:             $WX_CB_BUILD_DIR    |"
fi

echo "+-------------------------------------------------------------------------------------------------+"

# -------------------------------------------------------------------------------------------
# Copy the compiler DLL and wxWidget DLL's into the src/devel31_${BUILD_BITS} (32 or 64) directory
# -------------------------------------------------------------------------------------------
if [ "${OSDetected}" = "Windows" ] ; then
    echo "|                                                                                                 |"
    echo "|    Copying compiler and wxWidget DLL's into the src/devel31_${BUILD_BITS} directory.                       |"

    if [ "${BUILD_BITS}" == "32" ]; then
        if [ -f  "${GCC_ROOT}/libgcc_s_dw2-1.dll" ]; then
            cp -f "${GCC_ROOT}/libgcc_s_dw2-1.dll"        ${CB_DEVEL_DIR} > /dev/null
        fi
    fi    
    if [ "${BUILD_BITS}" == "64" ]; then
        if [ -f "${GCC_ROOT}/libgcc_s_seh-1.dll" ]; then
            cp -f "${GCC_ROOT}/libgcc_s_seh-1.dll"        ${CB_DEVEL_DIR} > /dev/null
        fi
        # The next DLL is required for some GCC compilers, but not for others. Either way copy it is if [ -f s.
        if [ -f  "${GCC_ROOT}/libgcc_s_seh_64-1.dll" ]; then
            cp -f "${GCC_ROOT}/libgcc_s_seh_64-1.dll"     ${CB_DEVEL_DIR} > /dev/null
        fi
    fi    

    [ -f  "${GCC_ROOT}/libwinpthread-1.dll" ]         && cp -f "${GCC_ROOT}/libwinpthread-1.dll"        ${CB_DEVEL_DIR} > /dev/null
    [ -f  "${GCC_ROOT}/libstdc++-6.dll" ]             && cp -f "${GCC_ROOT}/libstdc++-6.dll"            ${CB_DEVEL_DIR} > /dev/null
    [ -f  "${GCC_ROOT}/libbz2-1.dll" ]                && cp -f "${GCC_ROOT}/libbz2-1.dll"               ${CB_DEVEL_DIR} > /dev/null
    [ -f  "${GCC_ROOT}/zlib1.dll" ]                   && cp -f "${GCC_ROOT}/zlib1.dll"                  ${CB_DEVEL_DIR} > /dev/null
    [ ! -f "${CB_DEVEL_DIR}/exchndl.dll" ]  && cp -f -r "${CB_SRC}/exchndl/Win_10/win${BUILD_BITS}/bin/."  ${CB_DEVEL_DIR} > /dev/null

    count=$(ls ${GCC_ROOT}/libhunspell-*.dll 2>/dev/null | wc -l)
    if [ $count -gt 0 ] ; then
        for libHunspellDLLFile in ${GCC_ROOT}/libhunspell-*.dll
        do 
            cp -f $libHunspellDLLFile ${CB_DEVEL_DIR} > /dev/null
        done
    fi

    count=$(ls $WX_CB_BUILD_DIR/lib/gcc_dll/*.dll 2>/dev/null | wc -l)
    if [ $count -gt 0 ] ; then
        for wxFileDLL in $WX_CB_BUILD_DIR/lib/gcc_dll/*.dll
        do 
            cp $wxFileDLL  ${CB_DEVEL_DIR} > /dev/null
        done
    fi

    # ------------------------------------------------------------------------------------------
    # Rename DLL files if built with MSYS 2 using bootstrap/configure/make/make install process
    # ------------------------------------------------------------------------------------------
    count=$(ls -1 ${CB_DEVEL_DIR}/share/codeblocks/plugins/*.dll 2>/dev/null | wc -l)
    if [ $count -eq 0 ] ; then
        echo "|                                                                                                 |"
        echo "|             +-----------------------------------------------------------+                       |"
        echo "|             | Error: Code::Blocks Plugin DLL files not found in:        |                       |"
        echo "|             |          src/devel31_${BUILD_BITS}/share/codeblocks/plugins          |                       |"
        echo "|             +-----------------------------------------------------------+                       |"
        echo "|                                                                                                 |"
        echo "|                                                                                                 |"
        echo "+-------------------------------------------------------------------------------------------------+"
        echo
        cd ${InitialDir}
        exit 8
    fi

    count=$(ls -1 ${CB_DEVEL_DIR}/share/codeblocks/plugins/lib*.dll 2>/dev/null | wc -l)
    if [ $count -gt 0 ] ; then
        PrevDirectory=$PWD
        cd "${CB_DEVEL_DIR}/share/codeblocks/plugins"
        for pluginLibFile in lib*.dll
        do 
            newPluginLibFile=$(echo $pluginLibFile |sed 's/^.\{3\}//g')
            [ -f $newFilename ] && rm -f $newPluginLibFile
            mv $pluginLibFile $newPluginLibFile
        done
        cd $PrevDirectory
    fi

    # -------------------------------------------------------------------------------------------------------
    # Delete DLL l*.la redundant files built with MSYS 2 using bootstrap/configure/make/make install process
    # -------------------------------------------------------------------------------------------------------
    count=$(ls -1 ${CB_DEVEL_DIR}/*.la 2>/dev/null | wc -l)
    if [ $count -gt 0 ] ; then
        PrevDirectory=$PWD
        cd "${CB_DEVEL_DIR}"
        find . -type f -name "*.la" | xargs rm -f
        cd $PrevDirectory
    fi

    # -------------------------------------------------------------------------------------------------------
    # Delete Plugin DLL *.a redundant files built with MSYS 2 using bootstrap/configure/make/make install process
    # -------------------------------------------------------------------------------------------------------
    count=$(ls -1 ${CB_DEVEL_DIR}/share/CodeBlocks/plugins/*.a 2>/dev/null | wc -l)
    if [ $count -gt 0 ] ; then
        PrevDirectory=$PWD
        cd "${CB_DEVEL_DIR}/share/CodeBlocks/plugins"
        find . -type f -name "*.a" | xargs rm -f
        cd $PrevDirectory
    fi
else    
    copyImageFiles() {
        from=$1
        to=$2
        echo "Copy ${from} to ${to}"

        # This is done this stupid/convoluted way, because dash/posix doesn't have arrays!
        dirs="16x16 20x20 24x24 28x28 32x32 40x40 48x48 56x56 64x64"
        echo "$dirs" | tr ' ' '\n' | while read dir; do
            echo "  Copy ${from}/${dir} to ${to}/${dir}"

            mkdir -p ${to}/${dir}
            cp -f ${from}/${dir}/*.png ${to}/${dir} > /dev/null
        done
    }

    CB_DEVEL_RESDIR=${CB_DEVEL_DIR}/share/codeblocks

    mkdir -p ${CB_DEVEL_RESDIR}/compilers
    mkdir -p ${CB_DEVEL_RESDIR}/lexers
    mkdir -p ${CB_DEVEL_RESDIR}/images/settings
    mkdir -p ${CB_DEVEL_RESDIR}/plugins
    mkdir -p ${CB_DEVEL_RESDIR}/templates
    mkdir -p ${CB_DEVEL_RESDIR}/templates/wizard
    mkdir -p ${CB_DEVEL_RESDIR}/scripts/tests
    mkdir -p ${CB_DEVEL_RESDIR}/docs

    ZIPCMD="zip"

    echo "|                                                                                                 |"
    echo "| Compressing core UI resources                                                                   |"
    ${ZIPCMD} -jqu9 ${CB_DEVEL_RESDIR}/resources.zip            ${CB_ROOT}/src/src/resources/*.xrc > /dev/null
    ${ZIPCMD} -jqu9 ${CB_DEVEL_RESDIR}/manager_resources.zip    ${CB_ROOT}/src/sdk/resources/*.xrc sdk/resources/images/*.png > /dev/null
    ${ZIPCMD} -jqu9 ${CB_DEVEL_RESDIR}/start_here.zip           ${CB_ROOT}/src/src/resources/start_here/* > /dev/null

    echo "| Compressing plugins UI resources                                                                |"
    ${ZIPCMD} -jqu9 ${CB_DEVEL_RESDIR}/Astyle.zip           ${CB_ROOT}/src/plugins/astyle/resources/manifest.xml                        ${CB_ROOT}/src/plugins/astyle/resources/*.xrc               > /dev/null
    ${ZIPCMD} -jqu9 ${CB_DEVEL_RESDIR}/autosave.zip         ${CB_ROOT}/src/plugins/autosave/manifest.xml                                ${CB_ROOT}/src/plugins/autosave/*.xrc                       > /dev/null
    ${ZIPCMD} -jqu9 ${CB_DEVEL_RESDIR}/classwizard.zip      ${CB_ROOT}/src/plugins/classwizard/resources/manifest.xml                   ${CB_ROOT}/src/plugins/classwizard/resources/*.xrc          > /dev/null
    ${ZIPCMD} -jqu9 ${CB_DEVEL_RESDIR}/clangd_client.zip    ${CB_ROOT}/src/plugins/clangd_client/src/resources/manifest.xml             ${CB_ROOT}/src/plugins/clangd_client/src/resources/*.xrc   > /dev/null
    ${ZIPCMD} -jqu9 ${CB_DEVEL_RESDIR}/compiler.zip         ${CB_ROOT}/src/plugins/compilergcc/resources/manifest.xml                   ${CB_ROOT}/src/plugins/compilergcc/resources/*.xrc          > /dev/null
    ${ZIPCMD} -jqu9 ${CB_DEVEL_RESDIR}/debugger.zip                 ${CB_ROOT}/src/plugins/debuggergdb/resources/manifest.xml           ${CB_ROOT}/src/plugins/debuggergdb/resources/*.xrc          > /dev/null
    ${ZIPCMD} -jqu9 ${CB_DEVEL_RESDIR}/defaultmimehandler.zip       ${CB_ROOT}/src/plugins/defaultmimehandler/resources/manifest.xml    ${CB_ROOT}/src/plugins/defaultmimehandler/resources/*.xrc   > /dev/null
    ${ZIPCMD} -jqu9 ${CB_DEVEL_RESDIR}/occurrenceshighlighting.zip  ${CB_ROOT}/src/plugins/occurrenceshighlighting/resources/*.xrc      ${CB_ROOT}/src/plugins/occurrenceshighlighting/resources/manifest.xml > /dev/null
    ${ZIPCMD} -jqu9 ${CB_DEVEL_RESDIR}/openfileslist.zip            ${CB_ROOT}/src/plugins/openfileslist/manifest.xml                                                                               > /dev/null
    ${ZIPCMD} -jqu9 ${CB_DEVEL_RESDIR}/projectsimporter.zip         ${CB_ROOT}/src/plugins/projectsimporter/resources/*.xrc             ${CB_ROOT}/src/plugins/projectsimporter/resources/manifest.xml > /dev/null
    ${ZIPCMD} -jqu9 ${CB_DEVEL_RESDIR}/scriptedwizard.zip           ${CB_ROOT}/src/plugins/scriptedwizard/resources/manifest.xml                                                                    > /dev/null
    ${ZIPCMD} -jqu9 ${CB_DEVEL_RESDIR}/todo.zip                     ${CB_ROOT}/src/plugins/todo/resources/manifest.xml                  ${CB_ROOT}/src/plugins/todo/resources/*.xrc                 > /dev/null
    ${ZIPCMD} -jqu9 ${CB_DEVEL_RESDIR}/abbreviations.zip            ${CB_ROOT}/src/plugins/abbreviations/resources/manifest.xml         ${CB_ROOT}/src/plugins/abbreviations/resources/*.xrc        > /dev/null

    echo "| Packing core UI bitmaps                                                                         |"
    cd ${CB_ROOT}/src/src/resources
    ${ZIPCMD} -0 -qu ${CB_DEVEL_RESDIR}/resources.zip \
        images/*.png \
        images/16x16/*.png \
        images/20x20/*.png \
        images/24x24/*.png \
        images/28x28/*.png \
        images/32x32/*.png \
        images/40x40/*.png \
        images/48x48/*.png \
        images/56x56/*.png \
        images/64x64/*.png \
        images/tree/16x16/*.png \
        images/tree/20x20/*.png \
        images/tree/24x24/*.png \
        images/tree/28x28/*.png \
        images/tree/32x32/*.png \
        images/tree/40x40/*.png \
        images/tree/48x48/*.png \
        images/tree/56x56/*.png \
        images/tree/64x64/*.png \
        images/infopane/16x16/*.png \
        images/infopane/20x20/*.png \
        images/infopane/24x24/*.png \
        images/infopane/28x28/*.png \
        images/infopane/32x32/*.png \
        images/infopane/40x40/*.png \
        images/infopane/48x48/*.png \
        images/infopane/56x56/*.png \
        images/infopane/64x64/*.png \
        > /dev/null
    cd ${CB_ROOT}/src/sdk/resources
    ${ZIPCMD} -0 -qu ${CB_DEVEL_RESDIR}/manager_resources.zip \
        images/*.png \
        images/8x8/*.png \
        images/10x10/*.png \
        images/12x12/*.png \
        images/16x16/*.png \
        images/20x20/*.png \
        images/24x24/*.png \
        images/28x28/*.png \
        images/32x32/*.png \
        images/40x40/*.png \
        images/48x48/*.png \
        images/56x56/*.png \
        images/64x64/*.png \
        > /dev/null
    echo "| Packing plugins UI bitmaps                                                                      |"
    cd ${CB_ROOT}/src/plugins/compilergcc/resources
    ${ZIPCMD} -0 -qu ${CB_DEVEL_RESDIR}/compiler.zip \
        images/16x16/*.png \
        images/20x20/*.png \
        images/24x24/*.png \
        images/28x28/*.png \
        images/32x32/*.png \
        images/40x40/*.png \
        images/48x48/*.png \
        images/56x56/*.png \
        images/64x64/*.png \
        > /dev/null
    cd ${CB_ROOT}/src/plugins/contrib/clangd_client/src/resources
    ${ZIPCMD} -0 -qu ${CB_DEVEL_RESDIR}/clangd_client.zip \
        images/16x16/*.png \
        images/20x20/*.png \
        images/24x24/*.png \
        images/28x28/*.png \
        images/32x32/*.png \
        images/40x40/*.png \
        images/48x48/*.png \
        images/56x56/*.png \
        images/64x64/*.png \
        > /dev/null
    cd ${CB_ROOT}/src/plugins/abbreviations/resources
    ${ZIPCMD} -0 -qu ${CB_DEVEL_RESDIR}/abbreviations.zip \
        images/16x16/*.png \
        images/20x20/*.png \
        images/24x24/*.png \
        images/28x28/*.png \
        images/32x32/*.png \
        images/40x40/*.png \
        images/48x48/*.png \
        images/56x56/*.png \
        images/64x64/*.png \
        > /dev/null
    cd ${CB_ROOT}

    echo "| Copying files                                                                                   |"
    # Create an exclude pattern file
    echo .svn > excludes.txt
    echo Makefile >> excludes.txt
    echo Makefile.am >> excludes.txt
    echo Makefile.in >> excludes.txt

    cp -f ${CB_ROOT}/src/sdk/resources/lexers/lexer_*                                       ${CB_DEVEL_RESDIR}/lexers > /dev/null
    cp -f ${CB_ROOT}/src/src/resources/images/*.png                                         ${CB_DEVEL_RESDIR}/images > /dev/null
    cp -f ${CB_ROOT}/src/src/resources/images/settings/*.png                                ${CB_DEVEL_RESDIR}/images/settings > /dev/null
    cp -f ${CB_ROOT}/src/plugins/compilergcc/resources/compilers/*.xml                      ${CB_DEVEL_RESDIR}/compilers > /dev/null
    rsync -au --exclude-from=excludes.txt ${CB_ROOT}/src/plugins/scriptedwizard/resources/  ${CB_DEVEL_RESDIR}/templates/wizard > /dev/null
    rsync -au --exclude-from=excludes.txt ${CB_ROOT}/src/templates/common/                  ${CB_DEVEL_RESDIR}/templates > /dev/null
    rsync -au --exclude-from=excludes.txt ${CB_ROOT}/src/templates/unix/                    ${CB_DEVEL_RESDIR}/templates > /dev/null
    cp -f ${CB_ROOT}/src/scripts/*.script                                                   ${CB_DEVEL_RESDIR}/scripts > /dev/null
    cp -f ${CB_ROOT}/src/scripts/tests/*.script                                             ${CB_DEVEL_RESDIR}/scripts/tests > /dev/null

    rm excludes.txt
    
    if [ -d "${CB_ROOT}/windows_installer" ]; then
        echo "| Copying documentation                                                                           |"
        cp  ${CB_ROOT}/windows_installer/Documentation/*.* ${CB_DEVEL_RESDIR}/docs > /dev/null
    else
        cp  ${CB_SRC}/plugins/contrib/help_plugin/index.ini ${CB_DEVEL_RESDIR}/docs
    fi

    cd ${CB_ROOT}
fi

# ------------------------------------------------------------------------------------------

echo "|                                                                                                 |"
echo "|                      Completed updating C::B directory build files.                             |"
echo "|                                                                                                 |"
echo "+-------------------------------------------------------------------------------------------------+"
echo

cd ${InitialDir}

read -r -p 'Would you like to run Code::Blocks? ( type "y" for yes or "n" for no ): '
if [ "${REPLY}" == 'y' ] || [ "${REPLY}" == 'Y' ]; then
    case ${OSDetected} in
        OSX)
            ./StartCB_Devel.sh
            ;;
        Linux)
            ./Start_CB.sh
            ;;
        Windows)
            ${CB_DEVEL_DIR}/codeblocks${EXEEXT} -v --prefix ${CB_DEVEL_DIR}
            ;;
        *)
            echo "Unknown OSDetected: ${OSDetected}"
            exit 0
        ;;
esac

fi
