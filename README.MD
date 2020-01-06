
# libe4

![Teserakt AG](logo.png)

  * [Introduction](#introduction)
  * [When to use](#when-to-use)
  * [Building](#building)
  * [Integration](#integration)
     + [Compilation](#compilation)
     + [Storages](#storages)
     + [Concurrency](#concurrency)
     + [Cross-compilation](#cross-compilation)
  * [Contributing](#contributing)
  * [Security](#security)
  * [Support](#support)
  * [Intellectual property](#intellectual-property)


## Introduction

This repository provides the `libe4` C library, a client library for 
[Teserakt's E4](https://teserakt.io/e4.html), and end-to-end encryption and 
key management framework for MQTT and other publish-subscribe protocols.

`libe4` defines a simple interface for encryption and decryption of messages.

 * `int e4c_protect_message(uint8_t *ciphertext,
                        size_t ciphertext_max_len,
                        size_t *ciphertext_len,
                        const uint8_t *message,
                        size_t message_len,
                        const char *topic_name,
                        e4storage *storage);` - this function takes a message 
   to be protected and a "topic" for which it should be protected and returns 
   ciphertext that can be sent using your messaging layer.
 * `int e4c_unprotect_message(uint8_t *message,
                          size_t message_max_len,
                          size_t *message_len,
                          const uint8_t *ciphertext,
                          size_t ciphertext_len,
                          const char *topic_name,
                          e4storage *storage);`
    This function performs the reverse of the protect function. 

We talk of message *protection* instead of just *encryption* because the 
protection operation includes also authentication and replay defense.

E4's server (C2) is necessary to send control messages and manage a fleet of 
clients through GUIs, APIs, and automation components. The server can for 
example deploy key rotation policies, grant and revoke rights, and enable 
forward secrecy.

Please [contact us](mailto:contact@teserakt.io) to request access to a private 
instance of the server, or test the limited public version. Without the C2 
server, the E4 client library can be used to protect messages using static 
keys, manually managed.

## When to use

This code implements the same protocol as [`e4go`](https://github.com/teserakt-io/e4go/). 
If you are using a system that can support projects written in Golang, you 
may prefer to benefit from the security and reliability of the language 
instead of using C code.

This project is aimed at cases that have very small memory footprints, 
cannot tolerate stop-the-world garbage collection (i.e. realtime applications) 
or cannot run Golang code.

## Building

`libe4` is designed only to include those components that are necessary for 
a given mode to run. In particular, for low end devices using only symmetric 
encryption, public key variants of the code should not be used.

`libe4` can be compiled as follows:

    CONF=symkey make

This will output, into the `build/symkey` folder, a directory structure 
as follows:

```
.
├── include
│   └── e4
│       ├── crypto
│       │   ├── aes256enc.h
│       │   ├── aes_siv.h
│       │   ├── curve25519.h
│       │   ├── ed25519.h
│       │   ├── fixedint.h
│       │   ├── selftest.h
│       │   ├── sha3.h
│       │   ├── sha512.h
│       │   └── xed25519.h
│       ├── e4.h
│       ├── inline.h
│       ├── internal
│       │   ├── e4c_pk_store_file.h
│       │   └── e4c_store_file.h
│       ├── pstdint.h
│       ├── stdint.h
│       ├── strlcpy.h
│       └── util.h
├── lib
│   └── libe4.a
```

Tests can be built with the following command:

    CONF=symkey make testbuild

and the following command will build and execute tests in one pass

    CONF=symkey make test

Users who wish to build the pubkey variant should run:

    CONF=pubkey make

and where needed `make test`. Output in this case will be in `build/pubkey`.

## Integration

### Compilation

Once compiled, `libe4` can be integrated into your application as follows:

 * You should add `build/mode/include` to your include path, e.g. 
   `-I$(E4DIR)/build/symkey/include`.
 * You should link with the static library.

You may then use `e4` in your code with the include:

    #include "e4/e4.h"

### Storages

`libe4` is designed to be used in environments that may not include any kind 
of runtime or kernel. As a consequence, implementing persistent storage 
will depend entirely on the capabilities of the hardware environment.

To overcome this, `libe4` provides a `e4storage` struct and corresponding 
set of functions that are forward-declared in `e4.h`. It also provides a 
demonstration "file store" for both variants, and a "memory store" that is 
not persistent.

The file store is enabled by default. To configure it explicitly, run:

    CONF=symkey STORE=file make

To select memory storage, run

    CONF=pubkey STORE=mem make

(this is also using public keys).

Alternatively, you may implement the storage APIs yourself. These are 
described in [`e4.h`](include/e4/e4.h). If you do this, be sure to 
pass 

    CONF=... STORE=none make

### Concurrency

`libe4` currently makes zero effort to be thread-safe and function calls are 
not reentrant. This is because we wish to have zero dependencies on APIs 
that provide this functionality and we have no knowledge of the target CPU 
architecture.

### Cross-compilation

`libe4` respects environment variables such as `CC`. You may therefore, 
for example, cross compile for arm using something like:

    CC=arm-none-eabi-gcc CONF=symkey STORE=mem make

## Contributing

Before contributing, please read our [CONTRIBUTING](CONTRIBUTING.md) guide.

## Security

To report a security vulnerability (or potential vulnerability where 
private discussion is preferred) see [SECURITY](SECURITY.MD).

## Intellectual Property

libe4 is &copy; Teserakt AG 2018-2020 and is released under the Apache 
License 2.0. `libe4` also uses open source components from various projects and 
these may be available to you under different licenses. Please see: 
[LICENSE](LICENSE.MD) for the license text and licenses of third party 
components.

