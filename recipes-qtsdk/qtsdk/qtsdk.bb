SUMMARY = "qt sdk"
DESCRIPTION = "qt sdk"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"


#include recipes-qt/meta/meta-toolchain-qt6.bb
#DEPENDS += "meta-toolchain-qt6"

inherit populate_sdk populate_sdk_qt6

DEPENDS:remove = "qtpdf"
DEPENDS:remove = "qtwebengine"
DEPENDS:remove = "python3-lxml"
DEPENDS:remove = "python3-lxml-native"
DEPENDS:remove = "gn-native_git"
BBMASK:append = " poky/meta/recipes-devtools/python/python3-lxml_5.0.0.bb"
BBMASK:append = " meta/recipes-devtools/python/python3-lxml_5.0.0.bb"
BBMASK:append = " meta-qt6/recipes-qt/qt6/gn-native_git.bb"