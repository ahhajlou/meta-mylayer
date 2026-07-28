SUMMARY = "RAUC GDBus Client Application"
DESCRIPTION = "A D-Bus client for RAUC using GDBus library to monitor and control RAUC operations"
LICENSE = "CLOSED"

# Source location - adjust based on your setup
SRC_URI = "file://rauc_gdbus_client.c \
           file://Makefile \
          "

# Source directory
S = "${WORKDIR}/sources"
UNPACKDIR = "${S}"

# Dependencies
DEPENDS = "glib-2.0"
RDEPENDS:${PN} = "glib-2.0 rauc dbus"

inherit pkgconfig

# Compile and install
do_compile() {
    ${CC} ${LDFLAGS} ${UNPACKDIR}/rauc_gdbus_client.c -o ${B}/rauc_gdbus_client `pkg-config --cflags --libs gio-2.0`
    # Build using Makefile
    #oe_runmake all
}

do_install() {
    # Install binary
    install -d ${D}${bindir}
    install -m 755 ${B}/rauc_gdbus_client ${D}${bindir}/rauc-gdbus-client
}

# Install additional files if needed
FILES:${PN} += "${bindir}/rauc-gdbus-client"
