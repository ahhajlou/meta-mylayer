FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
SRC_URI += " \
    file://logo_linux_clut224.ppm \
    file://myfrag.cfg \
    "


do_configure:prepend() {
    install -m 0644 ${UNPACKDIR}/logo_linux_clut224.ppm ${S}/drivers/video/logo/logo_linux_clut224.ppm
}