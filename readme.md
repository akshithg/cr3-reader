# CR3 Reader

A kernel module to read the CR3 register and process information.

## Usage

```bash
cd src # Change to the source directory
make # Build the kernel module
sudo insmod cr3_reader.ko # Insert the kernel module
dmesg | grep cr3reader # Check the kernel module output
sudo rmmod cr3_reader # Remove the kernel module
```

> NOTE: using the Makefile target `ins: sudo insmod cr3_reader.ko` raises a
> `module verification error`. But executing the command `sudo insmod
> cr3_reader.ko` directly works fine.
