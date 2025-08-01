DESCRIPTION = "Mainline Linux kernel"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

inherit kernel
S = "${WORKDIR}/git"
#SRC_URI += "file://defconfig"
PROVIDES = "virtual/kernel"

# Add this line to local.conf ot distro config
# PREFERRED_PROVIDER_virtual/kernel = "linux-mainline"`
