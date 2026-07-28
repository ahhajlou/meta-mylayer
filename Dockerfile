FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive
ARG TZ=Asia/Tehran
ARG YOCTO_BRANCH=scarthgap
ARG YOCTO_QT_BRANCH=6.8

RUN apt-get update \
 && apt-get upgrade -y \
 && apt-get install -y sudo tmux \
 && useradd -G sudo -m --shell /bin/bash yocto \
 && echo "%sudo ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers \
 && echo "yocto:yocto" | chpasswd


RUN apt-get install -y gawk wget git diffstat unzip texinfo gcc build-essential chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 python3-subunit zstd liblz4-tool file locales libacl1

RUN locale-gen en_US.UTF-8

#USER yocto:yocto

RUN mkdir /opt/yocto-docker \
  && chown yocto:yocto -R /opt/yocto-docker  
#    && cd /opt/yocto-docker \
#    && git clone -b ${YOCTO_BRANCH} git://git.yoctoproject.org/poky \
#    && git clone -b ${YOCTO_BRANCH} https://git.openembedded.org/meta-openembedded \
#    && git clone -b ${YOCTO_BRANCH} git://git.yoctoproject.org/meta-raspberrypi \
#    && git clone -b ${YOCTO_QT_BRANCH} git://code.qt.io/yocto/meta-qt6.git

VOLUME ["/opt/yocto-docker"]

ENTRYPOINT ["/bin/bash"]
    
