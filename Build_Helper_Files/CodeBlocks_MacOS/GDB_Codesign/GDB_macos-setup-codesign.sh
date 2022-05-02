#!/bin/bash

# This script is copied from https://github.com/llvm/llvm-project/blob/master/lldb/scripts/macos-setup-codesign.sh

if [ "$(id -u)" != "0" ]; then
    echo "You are not root. Please run again as root!!!"
    exit 1
fi

if [ ! -d $TMPDIR ] || [ "x$TMPDIR" == "x" ]; then
    if [ -d "/tmp" ]; then
        TMPDIR=/tmp
    fi
fi

if [ ! -d $TMPDIR ] || [ "x$TMPDIR" == "x" ]; then
    echo "Cannot find \$TMPDIR directory: $TMPDIR!!!"
    exit 1
else    
    echo "The \$TMPDIR directory is: $TMPDIR!!!"
fi

CERT="gdb_codesign"

function error() {
    echo error: "$@" 1>&2
    exit 1
}

function cleanup {
    # Remove generated files
    rm -f "$TMPDIR/$CERT.tmpl" "$TMPDIR/$CERT.cer" "$TMPDIR/$CERT.key" > /dev/null 2>&1
}

function codesign_gdb_files {
    # Kill task_for_pid access control daemon
    sudo pkill -f /usr/libexec/taskgated > /dev/null 2>&1
    sleep 10

    # Sign and entitle the gdb binary
    if [ -f "/usr/local/Cellar/gdb/11.2/bin/gdb" ]; then
        sudo codesign --entitlements gdb-entitlement.xml --force --sign ${CERT} /usr/local/Cellar/gdb/11.2/bin/gdb
        [ $? -eq 0 ] || error Something went wrong when Codesigning /usr/local/Cellar/gdb/11.2/bin/gdb
    fi
    sudo codesign --entitlements gdb-entitlement.xml --force --sign ${CERT} $(which gdb)
    [ $? -eq 0 ] || error Something went wrong when Codesigning $(which gdb)

}

trap cleanup EXIT

# Check if the certificate is already present in the system keychain
security find-certificate -Z -p -c "$CERT" /Library/Keychains/System.keychain > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo Certificate has already been generated and installed. Now going to use it to sign GDB.
    codesign_gdb_files
    exit 0
fi

# Create the certificate template
cat <<EOF >$TMPDIR/$CERT.tmpl
[ req ]
default_bits       = 2048        # RSA key size
encrypt_key        = no          # Protect private key
default_md         = sha512      # MD to use
prompt             = no          # Prompt for DN
distinguished_name = codesign_dn # DN template
[ codesign_dn ]
commonName         = "$CERT"
[ codesign_reqext ]
keyUsage           = critical,digitalSignature
extendedKeyUsage   = critical,codeSigning
EOF

echo Generating and installing gdb_codesign certificate

# Generate a new certificate
openssl req -new -newkey rsa:2048 -x509 -days 3650 -nodes -config "$TMPDIR/$CERT.tmpl" -extensions codesign_reqext -batch -out "$TMPDIR/$CERT.cer" -keyout "$TMPDIR/$CERT.key" > /dev/null 2>&1
[ $? -eq 0 ] || error Something went wrong when generating the certificate

# Install the certificate in the system keychain
sudo security add-trusted-cert -d -r trustRoot -p codeSign -k /Library/Keychains/System.keychain "$TMPDIR/$CERT.cer" > /dev/null 2>&1
[ $? -eq 0 ] || error Something went wrong when installing the certificate

# Install the key for the certificate in the system keychain
sudo security import "$TMPDIR/$CERT.key" -A -k /Library/Keychains/System.keychain > /dev/null 2>&1
[ $? -eq 0 ] || error Something went wrong when installing the key

codesign_gdb_files

# Exit indicating the certificate is now generated and installed
exit 0
