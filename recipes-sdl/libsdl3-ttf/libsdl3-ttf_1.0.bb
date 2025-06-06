SUMMARY = "SDL TTF"

LICENSE = "Zlib"
LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=fbb0010b2f7cf6e8a13bcac1ef4d2455"

DEPENDS = "libsdl3 harfbuzz"

PROVIDES = "libsdl3-ttf"

SRC_URI = "git://github.com/libsdl-org/SDL_ttf.git;protocol=https;branch=release-3.2.x"

SRCREV = "a1ce3670aec736ecbf0936c43f2f0cc53aa61e5b"


S = "${WORKDIR}/git"

inherit cmake

FILES:${PN} += "${datadir}/licenses"
