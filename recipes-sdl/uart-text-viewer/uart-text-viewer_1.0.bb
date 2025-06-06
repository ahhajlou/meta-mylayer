# simple-sdl3-app_1.0.bb
SUMMARY = "Simple SDL3 Application"
DESCRIPTION = "A basic SDL3 application with animated background"
LICENSE = "CLOSED"

DEPENDS = "libsdl3 libsdl3-ttf pkgconfig-native"

SRC_URI = "file://main.c;subdir=src \
           file://CMakeLists.txt;subdir=src"

S = "${WORKDIR}/src"

inherit cmake

# If you prefer to use a simple Makefile instead of CMake
# inherit autotools-brokensep

FILES:${PN} = "${bindir}/simple-sdl3-app"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${B}/simple-sdl3-app ${D}${bindir}/
}

RDEPENDS:${PN} = "libsdl3 libsdl3-ttf"
