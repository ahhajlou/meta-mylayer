KAS_COMMAND ?= kas-container

# KAS_WORK_DIR = ~/yocto
KAS_CONTAINER_ENGINE ?= podman
KAS_CONTAINER_IMAGE_DISTRO ?= debian-bookworm

QEMUARM64_KAS_CONFIG_FILE := kas-qemuarm64.yml
QEMUARM64_KAS_BUILD_DIR := build-qemuarm64

RPI5_KAS_CONFIG_FILE := kas-rpi5.yml
RPI5_KAS_BUILD_DIR := build-rpi5


.PHONY: all help build-qemuarm64 build-raspberrypi5 exec-kas

all: help

define M_KAS_COMMAND
	$(eval M_KAS_CONFIG_FILE := $(if $(filter kas-container,$(KAS_COMMAND)),$(2):kas-container-conf.yml,$(2)))

	KAS_CONTAINER_ENGINE=$(KAS_CONTAINER_ENGINE) \
	KAS_CONTAINER_IMAGE_DISTRO=$(KAS_CONTAINER_IMAGE_DISTRO) \
	KAS_BUILD_DIR=$(QEMUARM64_KAS_BUILD_DIR) \
	$(KAS_COMMAND) $(1) $(M_KAS_CONFIG_FILE)
endef



build-qemuarm64:
	$(call M_KAS_COMMAND,build,$(QEMUARM64_KAS_CONFIG_FILE))

build-raspberrypi5:
	$(call M_KAS_COMMAND,build,$(RPI5_KAS_CONFIG_FILE))

exec-kas:
	KAS_CONTAINER_ENGINE=$(KAS_CONTAINER_ENGINE) \
	KAS_CONTAINER_IMAGE_DISTRO=$(KAS_CONTAINER_IMAGE_DISTRO) \
	KAS_BUILD_DIR=$(QEMUARM64_KAS_BUILD_DIR) \
	$(KAS_COMMAND) $(EKC)

help:
	@echo "Help"
