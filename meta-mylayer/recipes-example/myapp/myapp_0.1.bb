SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "Recipe created by bitbake-layers"
LICENSE = "CLOSED"

SRC_URI = "file://test123456789.dtbo;subdir=sources"
S = "${WORKDIR}/sources"
UNPACKDIR = "${S}"

inherit deploy

FILES:${PN} = ""
ALLOW_EMPTY:${PN} = "1"

do_deploy() {
    install -d ${DEPLOYDIR}/overlays
    install -m 0664 ${S}/test123456789.dtbo ${DEPLOYDIR}/overlays/test123456789.dtbo
}

addtask deploy after do_compile
