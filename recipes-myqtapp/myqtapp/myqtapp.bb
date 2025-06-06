SUMMARY = "This is my qt app"
DESCRIPTION = "Recipe created by QT app"
LICENSE = "CLOSED"


SRC_URI = "\
           file://myQTApp.cpp \
           file://myQTApp.pro \
           "

S = "${WORKDIR}/sources"

DEPENDS += " qtbase "

RDEPENDS:${PN} += "qtwayland"


inherit qt6-qmake

do_install:append() {
	install -d ${D}${bindir}
	install -m 0775 myQTApp ${D}${bindir}/
}

FILES:${PN} += "${bindir}/myQTApp"


