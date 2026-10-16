# Firmware recovery (Iris LM-K)

Build target: `keebio/iris_lm/k1`

## Bootloader (STM32 DFU)

Any of:

1. Hold the **physical reset** button on the PCB for **≥1 second**, then release
2. Hold the **top-left key** while plugging USB in (Bootmagic)
3. From a working keymap: **Lower + R** (left) or **Raise + Del** (right) → `QK_BOOT`

## Flash one half at a time

Plug **one half** into the Mac (the half with the USB cable). Put it in bootloader, then:

```bash
export PATH="/opt/homebrew/opt/arm-none-eabi-gcc@8/bin:/opt/homebrew/opt/arm-none-eabi-binutils/bin:$PATH"
qmk flash -kb keebio/iris_lm/k1 -km steno_qwerty
```

Unplug, plug the **other half**, enter bootloader again, run the same command.

## Restore stock-like layout without NKRO customization

```bash
qmk flash -kb keebio/iris_lm/k1 -km default
```

(`default` lives in upstream `qmk_firmware`, not this userspace repo.)

Keep copies of known-good `.bin` files in this folder after successful builds.
