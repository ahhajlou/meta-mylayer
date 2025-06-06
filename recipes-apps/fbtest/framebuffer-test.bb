SUMMARY = "Virtual keyboard"
DESCRIPTION = "${SUMMARY}"
LICENSE = "CLOSED"


SRC_URI = "file://Makefile \
           file://framebuffer-test.c \
          "            

S = "${WORKDIR}/sources"
UNPACKDIR = "${S}"


do_compile() {
    bbnote "Here =====>"
    ls -la ${S}
    #${CC} ${S}/framebuffer-test.c -o ${B}/framebuffer-test
    oe_runmake
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${B}/framebuffer-test ${D}${bindir}/
}

FILES:${PN} += "${bindir}/framebuffer-test"

PROVIDES = "framebuffer-test"
RPROVIDES:${PN} = "${PROVIDES}"
