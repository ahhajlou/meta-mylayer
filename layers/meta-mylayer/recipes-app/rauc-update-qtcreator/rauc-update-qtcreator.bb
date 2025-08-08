SUMMARY = "This is my qt 6 app"
DESCRIPTION = "Recipe created by QT app"
LICENSE = "CLOSED"

SRC_URI = "\
    file://main.cpp \
    file://mainwindow.cpp \
    file://mainwindow.h \
    file://mainwindow.ui \
    file://rauc_gdbus.c \
    file://rauc_gdbus.h \
    file://bloc_dev_monitor.h \
    file://bloc_dev_monitor.c \
    file://rauc-update-qtcreator.pro \
        "


S = "${WORKDIR}/sources"
UNPACKDIR = "${S}"


DEPENDS += "qtbase qtdeclarative libdrm mesa glib-2.0 eudev"
RDEPENDS:${PN} = "\
    qtbase-plugins \
    qtdeclarative-plugins \
    mesa \
    libdrm \
    glib-2.0 rauc dbus eudev \
    "


EXTRA_QMAKEVARS_PRE += "CONFIG+=debug"
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"

inherit qt6-qmake pkgconfig

do_install:append() {
	install -d ${D}${bindir}
	install -m 0775 ${B}/rauc-update-qtcreator ${D}${bindir}/

}


FILES:${PN} += "\
    ${bindir}/rauc-update-qtcreator \
    "


