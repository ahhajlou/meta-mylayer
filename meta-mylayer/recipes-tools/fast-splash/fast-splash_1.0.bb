SUMMARY = "Custom splash screen image for framebuffer"
DESCRIPTION = "Converts source image to framebuffer format using native FFmpeg"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"


# Source image file
SRC_URI = "file://splash_source.jpg"

# Dependencies - ImageMagick native and lzop native for build-time conversion
DEPENDS = "imagemagick-native lzop-native"

# No need for traditional source directory
S = "${WORKDIR}/sources"
UNPACKDIR = "${S}"

# Build-time variables
SPLASH_WIDTH ?= "1920"
SPLASH_HEIGHT ?= "1080"
SPLASH_FORMAT ?= "rgb565le"


do_compile() {
    # Convert source image to RGB using native ImageMagick
    ${STAGING_BINDIR_NATIVE}/magick.im7 convert ${S}/splash_source.jpg \
        -resize ${SPLASH_WIDTH}x${SPLASH_HEIGHT}! \
        -depth 8 \
        -colorspace sRGB \
        RGB:temp_rgb.raw
    
    # Convert RGB888 to RGB565 using Python (available in most Yocto builds)
    python3 -c "
import sys
import struct

# Read RGB data (8-bit per channel)
with open('${B}/temp_rgb.raw', 'rb') as f:
    rgb_data = f.read()

# Convert RGB888 to RGB565
rgb565_data = bytearray()
for i in range(0, len(rgb_data), 3):
    if i + 2 < len(rgb_data):
        r = rgb_data[i] >> 3      # 8 bits -> 5 bits
        g = rgb_data[i + 1] >> 2  # 8 bits -> 6 bits  
        b = rgb_data[i + 2] >> 3  # 8 bits -> 5 bits
        
        # Pack into 16-bit RGB565 (little endian)
        rgb565 = (r << 11) | (g << 5) | b
        rgb565_data.extend(struct.pack('<H', rgb565))

with open('${B}/splash.fb', 'wb') as f:
    f.write(rgb565_data)
"
    
    # Clean up temporary file
    rm -f ${B}/temp_rgb.raw
    
    # Compress using native lzop
    ${STAGING_BINDIR_NATIVE}/lzop -9 ${B}/splash.fb
}

do_install() {
    install -d ${D}${datadir}/splash
    install -m 0644 ${B}/splash.fb ${D}${datadir}/splash/
    install -m 0644 ${B}/splash.fb.lzo ${D}${datadir}/splash/
}

# Package the files
FILES:${PN} = "\
    ${datadir}/splash/splash.fb \
    ${datadir}/splash/splash.fb.lzo \
    "

# Create development package for the uncompressed version
#PACKAGES = "${PN} ${PN}-dev"
#FILES:${PN}-dev = "${datadir}/splash/splash.fb"

RDEPENDS:${PN} = "lzop"

