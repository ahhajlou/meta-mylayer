SUMMARY = "This is my qt 6 app"
DESCRIPTION = "Recipe created by QT app"
LICENSE = "CLOSED"

SRC_URI = "\
        file://myQTApp \
        file://myQTApp.cpp \
        file://CMakeLists.txt \
        file://Main.qml \
        file://eglfs_kms.json \
        "


S = "${WORKDIR}/sources"
UNPACKDIR = "${S}"


DEPENDS += "qtbase qtdeclarative qtdeclarative-native libdrm mesa"

RDEPENDS:${PN} += "\
    qtbase-plugins \
    qtdeclarative-plugins \
    mesa \
    libdrm \
    "

inherit qt6-cmake

do_install:append() {
    install -d ${D}${sysconfdir}/myQTApp-conf
    install -m 0755 ${UNPACKDIR}/eglfs_kms.json ${D}${sysconfdir}/myQTApp-conf

    # The executable is already installed by qt6-cmake, but we ensure it's in the right place
    # If needed, we can rename it here, but typically qt6-cmake handles installation

    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${UNPACKDIR}/myQTApp ${D}${sysconfdir}/init.d

    sed -i \
        -e 's:#MYQTAPP-APP-NAME#:myQTApp:g' \
        -e 's:#BINDIR#:${bindir}:g' \
        -e 's:#EGLFS-KMS-CONFIG-PATH#:${sysconfdir}/myQTApp-conf/eglfs_kms.json:g' \
        ${D}${sysconfdir}/init.d/myQTApp
}

inherit update-rc.d

INITSCRIPT_NAME = "myQTApp"
INITSCRIPT_PARAMS = "defaults 99"


FILES:${PN} += "\
    ${bindir}/myQTApp \
    ${sysconfdir}/init.d/myQTApp \
    ${sysconfdir}/myQTApp-conf/* \
    "


