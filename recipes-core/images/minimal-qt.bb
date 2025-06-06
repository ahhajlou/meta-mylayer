SUMMARY = "Minimal wayland image"
DESCRIPTION = "Minimal desktop image based on core-image-weston"

include recipes-graphics/images/core-image-weston.bb


IMAGE_INSTALL:append = " myqtapp"
IMAGE_INSTALL:append = " qtbase"
