SUMMARY = "Minimal wayland image"
DESCRIPTION = "Minimal desktop image based on core-image-weston"

include recipes-graphics/images/core-image-weston.bb

#IMAGE_INSTALL:append = " great-recipe-dtb"
#IMAGE_BOOT_FILES:append = " overlays/great-recipe-dtb.dtbo"


IMAGE_INSTALL:append = " \
    evtest \
    uart-text-viewer \
    framebuffer-test \
    xf86-video-fbdev \
    xf86-video-modesetting \
"

IMAGE_INSTALL:append = " \
    virtual-keyboard \
    virtual-keyboard-dtbo \
"

IMAGE_BOOT_FILES:append = " overlays/virtual-keyboard.dtbo"
