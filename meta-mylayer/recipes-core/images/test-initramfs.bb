SUMMARY = "Test initramfs image"
DESCRIPTION = "Small initramfs image for test"


# ==================# (? NOT SURE ?) #====================
#UBOOT_MKIMAGE_DTCOPTS ?= ""
#UBOOT_ENTRYSYMBOL ?= "0x0"
#UBOOT_LOADADDRESS ?= "0x0"

# Optional: customize the ramdisk image
#UBOOT_RD_ENTRYPOINT ?= "0x0"
#UBOOT_RD_LOADADDRESS ?= "0x0"
# ==================# (? NOT SURE ?) #====================

INITRAMFS_SCRIPTS ?= "\
                      initramfs-framework-base \
                      initramfs-module-setup-live \
                      initramfs-module-udev \
                      initramfs-module-install \
                     "

PACKAGE_INSTALL = "\
    ${INITRAMFS_SCRIPTS} \
    ${VIRTUAL-RUNTIME_base-utils} \
    udev \
    base-passwd \
    ${ROOTFS_BOOTSTRAP_INSTALL} \
    fast-splash \
    "


# Keep it minimal
IMAGE_FEATURES = ""

# Don't allow the initramfs to contain a kernel
PACKAGE_EXCLUDE = "kernel-image-*"

IMAGE_NAME_SUFFIX ?= ""
IMAGE_LINGUAS = ""

LICENSE = "MIT"

# cpio.gz.u-boot image contains
IMAGE_FSTYPES = "cpio.gz cpio.gz.u-boot"
inherit core-image

IMAGE_ROOTFS_SIZE ?= "8192"

# Use the same restriction as initramfs-module-install
COMPATIBLE_HOST = '(x86_64.*|i.86.*|arm.*|aarch64.*)-(linux.*|freebsd.*)'

