require linux-mainline.inc
SRC_URI += "git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git;branch=linux-6.14.y;protocol=https"
SRCREV = "38fec10eb60d687e30c8c6b5420d86e8149f7557" 

# Add this line to local.conf ot distro config
# PREFERRED_PROVIDER_virtual/kernel = "linux-mainline"
