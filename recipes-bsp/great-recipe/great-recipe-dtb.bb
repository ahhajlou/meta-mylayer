SUMMARY = "Custom UART echo device tree overlay"
LICENSE = "CLOSED"

inherit devicetree

COMPATIBLE_MACHINE = "raspberrypi5"
SRC_URI = "file://great-recipe-dtb.dts"

S = "${WORKDIR}/src"

do_deploy:append() {
    install -d ${DEPLOYDIR}/overlays
    install -m 0644 "${B}/great-recipe-dtb.dtbo" "${DEPLOYDIR}/overlays/great-recipe-dtb.dtbo"
}
