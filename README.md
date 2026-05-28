# MINIX Calc Server (MINIX 3.2.0)

A custom arithmetic server implementation for **MINIX 3.2.0** demonstrating communication between:

* User programs
* User-space servers
* The Process Manager (PM)
* Kernel space

This project was developed for the **CS4421: Operating Systems II** course at the **National University of Lesotho**.

---

## Overview

The project introduces a new MINIX service called `calc` that provides two arithmetic operations:

| Operation                        | Execution Path           |
| -------------------------------- | ------------------------ |
| Addition (`calc_add`)            | User-space system call   |
| Multiplication (`calc_multiply`) | User-space → Kernel call |

The goal of the assignment was to demonstrate how MINIX’s microkernel architecture handles:

* System calls
* Kernel calls
* Message passing
* User-space servers
* Process Manager integration
* Kernel extensions

---

## Features

* Custom MINIX server (`calc`)
* Boot-time server registration
* User-space addition service
* Kernel-level multiplication service
* Process Manager routing
* User-level C library wrappers
* Test application
* Full MINIX source integration

---

# Architecture

## Addition Flow

```text
User Program
     ↓
Process Manager (PM)
     ↓
Calc Server
     ↓
Result Returned
```

The addition operation is handled entirely in user space.

---

## Multiplication Flow

```text
User Program
     ↓
Process Manager (PM)
     ↓
Calc Server
     ↓
Kernel Call (SYS_MULTIPLY)
     ↓
Kernel
     ↓
Calc Server
     ↓
Result Returned
```

The multiplication operation demonstrates communication between user space and kernel space.

---

# Project Structure

```text
/usr/src/servers/calc/
├── main.c
├── Makefile

/usr/src/kernel/system/
├── do_multiply.c

/usr/src/lib/libc/sys-minix/
├── calc.c

/home/calc_test/
├── calc_lib.h
├── test.c
```

---

# Files Created

| File                          | Purpose                       |
| ----------------------------- | ----------------------------- |
| `servers/calc/main.c`         | Main calc server              |
| `kernel/system/do_multiply.c` | Kernel multiplication handler |
| `lib/libc/sys-minix/calc.c`   | User-level API                |
| `calc_test/calc_lib.h`        | Header file                   |
| `calc_test/test.c`            | Test application              |

---

# Files Modified

## User Space

* `/usr/src/servers/calc/Makefile`
* `/usr/src/servers/Makefile`
* `/usr/src/lib/libc/sys-minix/`
* `/usr/src/include/unistd.h`

## Process Manager

* `/usr/src/include/minix/callnr.h`
* `/usr/src/servers/pm/table.c`
* `/usr/src/servers/pm/misc.c`
* `/usr/src/servers/pm/proto.h`

## Kernel Space

* `/usr/src/kernel/system.c`
* `/usr/src/kernel/system.h`
* `/usr/src/kernel/system/Makefile.inc`
* `/usr/src/kernel/table.c`
* `/usr/src/include/minix/com.h`

## Reincarnation Server & Policy

* `/usr/src/servers/rs/table.c`
* `/usr/src/etc/system.conf`

---

# Installation

## 1. Copy the DS Server

```bash
cp -r /usr/src/servers/ds /usr/src/servers/calc
```

---

## 2. Remove Unneeded DS Files

```bash
rm /usr/src/servers/calc/store.h
rm /usr/src/servers/calc/store.c
rm /usr/src/servers/calc/main.c
```

---

## 3. Add the Calc Server Source

Create:

```text
/usr/src/servers/calc/main.c
```

and implement the calc server logic.

---

## 4. Register the Server

Modify:

```text
/usr/src/servers/Makefile
```

Add:

```makefile
calc
```

---

## 5. Add Boot Image Integration

Modify:

```text
/usr/src/tools/Makefile
```

Add:

```makefile
../servers/calc/calc \
```

---

## 6. Define Process Number

Modify:

```text
/usr/src/include/minix/com.h
```

Add:

```c
#define CALC_PROC_NR ((endpoint_t) 11)
```

---

## 7. Add System Call Numbers

Modify:

```text
/usr/src/include/minix/callnr.h
```

Add:

```c
#define CALC_ADD 69
#define CALC_MULTIPLY 70
#define PM_CALC 97
```

---

## 8. Add Kernel Call

Create:

```text
/usr/src/kernel/system/do_multiply.c
```

---

## 9. Rebuild MINIX

```bash
cd /usr/src/releasetools
make hdboot
reboot
```

---

# Example Usage

## User Program

```c
#include <stdio.h>
#include "calc_lib.h"

int main()
{
    printf("3 + 5 = %d\n", calc_add(3,5));
    printf("9 * 2 = %d\n", calc_multiply(9,2));

    return 0;
}
```

---

# Example Output

```text
3 + 5 = 8
9 * 2 = 18
```

---

# Key Concepts Demonstrated

* Microkernel architecture
* Message passing
* System calls
* Kernel calls
* Process Manager routing
* User-space servers
* Kernel extensions
* MINIX boot image integration

---

# Compatibility

This implementation is designed specifically for:

* **MINIX 3.2.0**

Other MINIX versions may require modifications due to differences in:

* Source tree structure
* Build system
* System call handling
* Kernel APIs

---

# Contributors

| Student Number | Name         |
| -------------- | ------------ |
| 202100154      | Mohloai, KD  |
| 202201138      | Botsane, KAB |
| 202201269      | Mahao, TM    |

---

# References

1. Berkeley RC — *Adding a New Server to MINIX 3*
   http://berkeleyrc.blogspot.com/2010/05/adding-new-server-to-minix-316-kernel.html

---

# License

This project was developed for educational purposes as part of a university operating systems course.
