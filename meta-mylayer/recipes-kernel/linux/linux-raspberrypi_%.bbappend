FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
SRC_URI += " \
    file://enable-ad7175.cfg \
    file://enable-max31865.cfg \
    "
