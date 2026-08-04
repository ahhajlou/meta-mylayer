SUMMARY = "Receive keycodes over UART and inject them as keyboard input events"
DESCRIPTION = "${SUMMARY}"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://LICENSE;md5=570a9b3749dd0463a1778803b12a6dce"

SRC_URI = "git://github.com/ahhajlou/uartkbd.git;protocol=https;branch=dev"

# Modify these as desired
PV = "1.0+git"
SRCREV = "ccf50c3cad54f46e6cb6dfb378a3fcd15d34d4fb"

S = "${WORKDIR}/git"


inherit module

EXTRA_OEMAKE += "KERNEL_SRC=${STAGING_KERNEL_DIR}"

# WARNING: uartkbd-1.0+git-r0 do_install: Module.symvers not found in /build/workspace/sources/uartkbd/
# WARNING: uartkbd-1.0+git-r0 do_install: Please consider setting MODULES_MODULE_SYMVERS_LOCATION to a
# WARNING: uartkbd-1.0+git-r0 do_install: directory below B to get correct inter-module dependencies
# Probable fix: `MODULES_MODULE_SYMVERS_LOCATION = "${S}/driver/build"`