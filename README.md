# meta-uartkbd

A Yocto layer (kas-based) for building a qemuarm64 image that includes the
`uartkbd` kernel driver, so a "virtual keyboard over UART" can be tested in QEMU
end to end.

It's one of three repos that work together:

- [uartkbd](https://github.com/ahhajlou/uartkbd), the kernel driver. Runs on the
  target.
- [uartkbd-evsend](https://github.com/ahhajlou/uartkbd-evsend), the PC program.
  Reads your keyboard and sends the events over UART.
- this repo, which takes care of building everything and running it in QEMU.

So the flow is: uartkbd-evsend reads the keyboard on the host, pushes the events
out over the serial line, and inside QEMU the uartkbd driver turns them back into
input events.

## What's in the repo

- `kas-*.yml` builds `core-image-minimal` for qemuarm64 from poky +
  meta-openembedded. There's also a Raspberry Pi 5 config, though that target
  isn't wired up yet.
- `layers/meta-uartkbd` holds the recipe that builds the uartkbd module, plus a
  kernel config fragment that enables the serdev bus (`CONFIG_SERIAL_DEV_BUS`).
- `device-tree/uartkbd-device.dts` is an overlay that adds the `amh,uartkbd` node
  to the second PL011 UART. See `docs/qemu-device-tree.md` for the manual,
  step-by-step version of the same thing.
- `run-qemu.sh` dumps and merges the device tree, then boots QEMU with a second
  serial port exposed as a PTY.
- `Makefile` wraps all of that in a few kas-container commands.

The image also ships `evtest` and `openssh`, which is handy for looking at the
generated input device on the target.

## Install requirements

- ubuntu / debian:

```bash
sudo apt install qemu-system-aarch64
```

- fedora:

```bash
sudo dnf install qemu-system-aarch64
```

The image is built through `kas`, which runs the build in a container. Install it
with `uv`:

```bash
curl -LsSf https://astral.sh/uv/install.sh | sh
uv tool install kas
```

`kas` also needs a container engine. The Makefile defaults to `podman`; to use
docker instead, export `KAS_CONTAINER_ENGINE=docker` before running make. `kas` and
`kas-container` are interchangeable: the Makefile defaults to `kas`, but you can
switch back to `kas-container` with a make argument or an environment variable:

```bash
make build-qemuarm64 KAS_COMMAND=kas-container
# or
KAS_COMMAND=kas-container make build-qemuarm64
```

## Building and running

```bash
make build-qemuarm64
make run-qemuarm64
```

The second command boots QEMU with a serial port exposed as the PTY
`/tmp/second_uart.soc`. Keep it running, open another terminal and point
uartkbd-evsend at it:

```bash
cargo build --release
sudo ./target/release/uartkbd-evsend -i /dev/input/event3 -t /tmp/second_uart.soc
```

Root is required because the program opens `/dev/input/eventX` and `/dev/ttyX`.
The uartkbd-evsend README has the details.

## Shell inside the container

```bash
EKC="shell kas-qemuarm64.yml:kas-container-conf.yml" make exec-kas
```

## License

GPL-2.0
