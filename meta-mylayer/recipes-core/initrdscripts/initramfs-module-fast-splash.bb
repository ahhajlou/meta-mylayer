SUMMARY = "display splash"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"
RDEPENDS:${PN} = "initramfs-framework-base fast-splash"


inherit allarch

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI = "file://display-splash"

S = "${WORKDIR}/sources"
UNPACKDIR = "${S}"

do_install() {
    install -d ${D}/init.d
    install -m 0755 ${S}/display-splash ${D}/init.d/70-display-splash
}

FILES:${PN} = "/init.d/70-display-splash"
