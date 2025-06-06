SUMMARY = "Custom Device Tree Overlay"
DESCRIPTION = "Device tree overlay for custom hardware"
LICENSE = "CLOSED"

COMPATIBLE_MACHINE = "raspberrypi5"

#inherit devicetree

SRC_URI = "file://very-special-file.dts;subdir=src"

S = "${WORKDIR}/src"

DEPENDS = "dtc-native"

do_compile() {
    ${STAGING_BINDIR_NATIVE}/dtc -@ -I dts -O dtb \
        -o ${B}/very-special-file.dtbo ${S}/very-special-file.dts
}

do_install() {
    install -d ${D}/boot/overlays
    install -m 0644 ${B}/very-special-file.dtbo ${D}/boot/overlays/very-special-file.dtbo
}

inherit deploy
do_deploy() {
    install -d ${DEPLOYDIR}/overlays
    install -m 0644 ${B}/very-special-file.dtbo ${DEPLOYDIR}/overlays/very-special-file.dtbo

}

do_deploy[dirs] = "${DEPLOYDIR} ${DEPLOY_DIR_IMAGE}"

addtask deploy before do_build after do_install

FILES:${PN} += "/boot/overlays/very-special-file.dtbo"

