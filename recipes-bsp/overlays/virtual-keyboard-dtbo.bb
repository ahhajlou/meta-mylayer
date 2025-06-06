SUMMARY = "Device tree overlay for virtual keyboard driver"
LICENSE = "CLOSED"

inherit devicetree

COMPATIBLE_MACHINE = "raspberrypi5"
SRC_URI = "file://virtual-keyboard.dts"

do_deploy:append() {
    install -d ${DEPLOYDIR}/overlays
    install -m 0644 "${B}/virtual-keyboard.dtbo" "${DEPLOYDIR}/overlays/virtual-keyboard.dtbo"
}