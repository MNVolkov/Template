#!/usr/bin/env python3

from pathlib import Path
from subprocess import Popen

def run(args, ignore_retcode=False, dry=False):
    print("Running", " ".join(args))
    if dry:
        return
    p = Popen(args)
    retcode = p.wait()
    if retcode and not ignore_retcode:
        raise RuntimeError("Command returned non-zero exit code")
    elif retcode:
        print("Error, ignoring")

EABI = "arm-none-eabi"

AS = f"{EABI}-as"
LD = f"{EABI}-ld"
OBJCOPY = f"{EABI}-objcopy"
GCC = f"{EABI}-gcc"
NM = f"{EABI}-nm"

label = "labelname"
with open("label.txt") as label_file:
    label = label_file.readline()

thisdir = Path(".").absolute()
LIB_BIP_PATH = thisdir.parent/ "libbip" / "libbip.a"

print("Compiling")
objfiles = []
for f in list(thisdir.glob("**/*.c")):
    GCC_OPT = (GCC + " -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fno-math-errno -c -Os -Wa,-R -Wall -fpie -pie -fpic -mthumb -mlittle-endian -ffunction-sections -fdata-sections").split()
    cmd = GCC_OPT + ["-I"+str(LIB_BIP_PATH.parent.absolute()), "-o", f.stem + ".o", f.stem + ".c"]
    run(cmd)
    objfiles.append(f.stem+".o")

ELFNAME = thisdir.name + ".elf"

LD_OPT = "-lm -lc -EL -N -Os --cref -pie --gc-sections"
cmd = [
    LD,
    "-Map",
    thisdir.name + ".map",
    "-o",
    ELFNAME,
    "--no-dynamic-linker"
] + objfiles + LD_OPT.split() + [str(LIB_BIP_PATH)]

print("Linking")
run(cmd)

with open("name.txt", "w") as label_file:
    label_file.write(thisdir.name)

elfsections = {
    "label": "label.txt",
    "name": "name.txt",
    "resources": "asset.res",
    "settings": "settings.bin",
}

print("Objcopy")
for section, filename in elfsections.items():
    filepath = thisdir / filename
    if filepath.is_file():
        cmd = [
            OBJCOPY,
            thisdir.name + ".elf",
            "--add-section",
            f".elf.{section}={filename}",
        ]
        run(cmd)
