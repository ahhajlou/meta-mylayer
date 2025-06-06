SUMMARY = "Custom Weston-based image"
DESCRIPTION = "Custom image based on core-image-weston"

DEPENDS += "rpi-bootfiles"

# Inherit the base image class
#inherit core-image
include recipes-core/images/core-image-minimal.bb



# Add the same packages that core-image-weston includes
IMAGE_FEATURES += "splash package-management x11-base x11-sato ssh-server-dropbear hwcodecs"

# Core Weston packages (same as core-image-weston)
CORE_IMAGE_EXTRA_INSTALL += " \
    weston \
    weston-init \
    weston-examples \
"

# Include your overlay recipe
#IMAGE_INSTALL:append = " very-special-file"

# Use DEPLOYPAYLOAD - this is the method that actually works
#DEPLOYPAYLOAD = "very-special-file.dtbo:overlays/very-special-file.dtbo"
#RPI_SDIMG_EXTRA_DEPENDS:append = " very-special-file:do_deploy"