SUMMARY = "Minimal framebuffer image"
DESCRIPTION = "Minimal image with fbdev support"

include recipes-core/images/core-image-minimal.bb

IMAGE_INSTALL:append = " xf86-video-fbdev xf86-video-modesetting"

IMAGE_INSTALL:append = " evtest"
#IMAGE_INSTALL:append = " virtual-keyboard"

#IMAGE_INSTALL:append = " virtual-keyboard-dtbo"
#IMAGE_BOOT_FILES:append = " overlays/virtual-keyboard.dtbo"
