SUMMARY = "An eglfs graphical image."
LICENSE = "CLOSED"
IMAGE_LINGUAS = " "


IMAGE_FEATURES += "ssh-server-dropbear"


IMAGE_INSTALL = "kernel-modules \
    packagegroup-core-boot \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    "

# Graphics and DRM support
IMAGE_INSTALL:append = " \
    mesa \
    libdrm \
    libdrm-radeon \
    libdrm-nouveau \
    "

# Fonts package
IMAGE_INSTALL:append = " \
    freetype \
    fontconfig \
    fontconfig-utils \
    ttf-dejavu-common \
    ttf-dejavu-sans \
    ttf-dejavu-serif \
    myqtapp \
    "
    
inherit core-image features_check

REQUIRED_DISTRO_FEATURES = "opengl udev pam"
CONFLICT_DISTRO_FEATURES = "x11 wayland"

IMAGE_ROOTFS_SIZE ?= "8192"
IMAGE_ROOTFS_EXTRA_SPACE:append = "${@bb.utils.contains("DISTRO_FEATURES", "systemd", " + 4096", "", d)}"