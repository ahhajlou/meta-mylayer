DESCRIPTION = "Sample application"
LICENSE = "CLOSED"

SRC_URI = "file://sample-app.c;subdir=sources"

S = "${WORKDIR}/sources"
UNPACKDIR = "{S}"

do_compile () {
    ${CC} -o ${B}/sample-app ${CFLAGS} ${LDFLAGS} ${S}/sample-app.c
}

do_install () {
    install -d ${D}${bindir}
    install -m 0755 ${B}/sample-app ${D}${bindir}/sample-app
}

FILES:${PN} = "${bindir}/sample-app"