SUMMARY = "RAUC GDBus Client Application"
DESCRIPTION = "A D-Bus client for RAUC using GDBus library to monitor and control RAUC operations"
LICENSE = "CLOSED"

# Source location - adjust based on your setup
SRC_URI = "file://eudev_device_monitor.c \
          "

# Source directory
S = "${WORKDIR}/sources"
UNPACKDIR = "${S}"

# Dependencies
DEPENDS = "eudev"
RDEPENDS:${PN} = "eudev"

inherit pkgconfig

# Compile and install
do_compile() {
    ${CC} ${LDFLAGS} ${UNPACKDIR}/eudev_device_monitor.c -o ${B}/eudev_device_monitor `pkg-config --cflags --libs libudev`
}

do_install() {
    # Install binary
    install -d ${D}${bindir}
    install -m 755 ${B}/eudev_device_monitor ${D}${bindir}/eudev-device-monitor
}

# Install additional files if needed
FILES:${PN} += "${bindir}/eudev-device-monitor"
