# HDMI configuration settings for enabling framebuffer in a minimal distro build
DISABLE_OVERSCAN = "1"
GPU_MEM = "128"
HDMI_DRIVE = "2"
CONFIG_HDMI_BOOST = "4"
HDMI_FORCE_HOTPLUG = "1"

RPI_EXTRA_CONFIG = "\
hdmi_safe=1 \
"

CMDLINE:append = " video=HDMI-A-1:1920x1080M@60D"
