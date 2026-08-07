# Adding the uartkbd node to QEMU's device tree manually

The uartkbd driver binds to a device tree node with the `amh,uartkbd` compatible
string. QEMU's virt machine doesn't ship that node, so it has to be added before
the guest boots.

You normally don't need any of this: `run-qemu.sh` dumps the device tree, merges
the overlay in `device-tree/uartkbd-device.dts`, and boots with the result. This
page is the manual version, mainly useful for understanding what the script does
or for hacking on the device tree by hand.

You need `dtc`, from the device-tree-compiler package:

```bash
sudo apt install device-tree-compiler
```

## 1. Dump the DTB from QEMU

QEMU can write out the device tree it would hand to the guest:

```bash
qemu-system-aarch64 -machine virt,dumpdtb=virt.dtb -cpu cortex-a57 ...
```

Pass the rest of your usual flags; the file is written where `dumpdtb=` points.

## 2. Convert DTB to DTS

```bash
dtc -I dtb -O dts -o virt.dts virt.dtb
```

## 3. Add the child node

Find the second PL011 UART, `pl011@9040000` (the first one is at `9000000`), and
put a child node inside it:

```dts
pl011@9040000 {
	clock-names = "uartclk", "apb_pclk";
	clocks = <0x8000 0x8000>;
	interrupts = <0x00 0x08 0x04>;
	reg = <0x00 0x9040000 0x00 0x1000>;
	compatible = "arm,pl011", "arm,primecell";

	my_serdev_device {
		compatible = "amh,uartkbd";
	};
};
```

The compatible string is what the driver's `of_match_table` looks for, so this is
all it takes for serdev to create a device and probe the driver.

## 4. Convert back to DTB

```bash
dtc -I dts -O dtb -o virt-changed.dtb virt.dts
```

## 5. Boot QEMU with the new DTB

```bash
qemu-system-aarch64 ... -dtb virt-changed.dtb
```

Wire up the UARTs with `-serial` so the second PL011 (the one we added the node
to) is reachable from the host:

```bash
-serial mon:stdio \
-chardev pty,id=p6,path=/tmp/second_uart.soc -serial chardev:p6 \
-dtb virt-changed.dtb
```

The order of the flags matters: the first `-serial` maps to the first UART, the
second to `pl011@9040000`. Add `-display none` to run headless (leave it out if
you want the graphical console too).

Once the guest is up, the driver should bind to the new node:

```
uartkbd: serdev_uartkbd_probe called
input: myevent as /devices/platform/9040000.pl011/serial0/serial0-0/input/input2
```
