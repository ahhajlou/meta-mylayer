SUMMARY = "Receive keycodes over UART and inject them as keyboard input events"
DESCRIPTION = "${SUMMARY}"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://LICENSE;md5=570a9b3749dd0463a1778803b12a6dce"

SRC_URI = "git://github.com/ahhajlou/uartkbd.git;protocol=https;branch=main"

# Modify these as desired
PV = "1.0+git"
SRCREV = "6c45f2bdc568e61f9db2c0d95a334e44b85650f7"

S = "${WORKDIR}/git"


inherit module

EXTRA_OEMAKE += "KERNEL_SRC=${STAGING_KERNEL_DIR}"
