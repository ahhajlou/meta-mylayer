SUMMARY = "A receipe to build device tree overlay"
LICENSE = "MIT"

inherit devicetree

COMPATIBLE_MACHINE = "raspberrypi5"
SRC_URI = "file://devicetree-overlay.dts"

do_deploy:append() {
    install -d ${DEPLOYDIR}/overlays
    install -m 0644 "${B}/devicetree-overlay.dtbo" "${DEPLOYDIR}/overlays/devicetree-overlay.dtbo"
}