#!/bin/bash
set -eo pipefail

TOPDIR="$(pwd)"

DEVICE_TREE_DIR="${TOPDIR}/device-tree"
DUMP_DTB_PATH="${DEVICE_TREE_DIR}/virt-dump.dtb"
MERGED_DTB_PATH="${DEVICE_TREE_DIR}/virt-merged.dtb"
DT_OVERLAY_PATH="${DEVICE_TREE_DIR}/uartkbd-device.dts"

SECOND_SERIAL_PTY_PATH="/tmp/second_uart.soc"


merge_device_tree() {
    cd "$DEVICE_TREE_DIR"
    dtc -@ -I dts -O dtb -o serial2.dtbo "$DT_OVERLAY_PATH"
    fdtoverlay -i "$DUMP_DTB_PATH" -o "$MERGED_DTB_PATH" serial2.dtbo
}

get_machine_param() {
    if [[ "$1" == "dump" ]]; then
        echo "virt,dumpdtb=${DUMP_DTB_PATH}" 
    else
        echo "virt"
    fi
}

run_qemu() {
    local dump_dtb="$1"

    [[ ! -d "$DEVICE_TREE_DIR" ]] && mkdir "$DEVICE_TREE_DIR"
    [[ -h "$SECOND_SERIAL_PTY_PATH" ]] && unlink "$SECOND_SERIAL_PTY_PATH"

    qemu_args=()
    qemu_args+=('-device' 'virtio-net-pci,netdev=net0,mac=52:54:00:12:35:02')
    qemu_args+=('-netdev' "user,id=net0,hostfwd=tcp:127.0.0.1:2222-:22,hostfwd=tcp:127.0.0.1:2323-:23,tftp=${BUILD_DIR}/tmp/deploy/images/qemuarm64")
    qemu_args+=('-object' 'rng-random,filename=/dev/urandom,id=rng0')
    qemu_args+=('-device' 'virtio-rng-pci,rng=rng0')
    qemu_args+=('-drive' "id=disk0,file=${BUILD_DIR}/tmp/deploy/images/qemuarm64/core-image-minimal-qemuarm64.rootfs.ext4,if=none,format=raw")
    qemu_args+=('-device' 'virtio-blk-pci,drive=disk0')
    qemu_args+=('-device' 'virtio-gpu-pci')
    qemu_args+=('-device' 'qemu-xhci')
    qemu_args+=('-device' 'usb-tablet')
    qemu_args+=('-device' 'usb-kbd')

    if [[ "$dump_dtb" == "true" ]]; then 
        qemu_args+=('-machine' "$(get_machine_param dump)")
    else
         qemu_args+=('-machine' "$(get_machine_param)")
         qemu_args+=('-dtb' "$MERGED_DTB_PATH")
    fi

    qemu_args+=('-cpu' 'cortex-a57')
    qemu_args+=('-smp' '4')
    qemu_args+=('-m' '256')
    qemu_args+=('-kernel' "${BUILD_DIR}/tmp/deploy/images/qemuarm64/Image")
    qemu_args+=('-append' 'root=/dev/vda rw mem=256M ip=dhcp console=ttyAMA0 console=hvc0 swiotlb=0')

    qemu_args+=('-serial' 'mon:stdio')
    qemu_args+=('-chardev' "pty,id=p6,path=${SECOND_SERIAL_PTY_PATH}")
    qemu_args+=('-serial' 'chardev:p6')

    echo "${qemu_args[@]}"
    qemu-system-aarch64 "${qemu_args[@]}"
}

if [[ -z "$BUILD_DIR" ]]; then
    echo "BUILD_DIR is not set"
    exit 1
fi

if ! command -v qemu-system-aarch64 >/dev/null 2>&1;then
    echo "qemu-system-aarch64 is not installed"
    exit 1
fi

case "${1:-}" in
    dump_merge_dt)
        echo "Dump and merge device tree"
        if [[ -f "$MERGED_DTB_PATH" ]]; then
            echo "$MERGED_DTB_PATH exists"
            exit 0
        fi
        run_qemu true
        merge_device_tree        
        ;;

    run)
        echo "Run qemu"
        run_qemu false
        ;;

    *)
        echo "Invalid option"
        echo "Help: ./run-qemu.sh dump_merge_dt|run"
        ;;
esac
