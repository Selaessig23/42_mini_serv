# 42_mini_serv
`mini_serv` is a small TCP chat server. It listens on `127.0.0.1` and
broadcasts each complete line received from one client to all other connected
clients.

## Project purpose

The aim of this project was to prepare for and pass the 42 Core Curriculum
final exam (2026), rather than to build a comprehensive IRC server. The
important learning goals were to repeat the basics of sockets, clients, and
message handling, and especially to understand how the `select()` function can
monitor several file descriptors in one server loop.

The implementation therefore focuses on the exam-style requirements and keeps
the surrounding infrastructure deliberately small.

## File structure

- `mini_serv.c` is the main program. It creates and binds the listening
  socket, tracks connected clients, calls `select()`, receives data, and
  broadcasts complete messages.
- `mini_serv_helpers.c` contains the two string-buffer helpers used by the
  server. They handle joining received fragments and extracting complete
  newline-terminated messages. These functions were provided by the subject
  or are based on the provided functions, so they are kept separate from the
  main server loop.
- `debug.h` contains the optional signal-handling declaration and the
  `DEBUG_PRINT` macro used by the debug Make targets.
- `main_helper.c` is a separate example of the pre-given socket setup. It is
  kept as a reference and is not compiled by the Makefile.

This split keeps the exam-oriented server logic easy to read while isolating
the provided helper functions and optional development code. The Makefile
only compiles `mini_serv.c` and `mini_serv_helpers.c`.

## Requirements

- A Unix-like environment
- A C compiler available as `cc` (for example, Clang or GCC)
- `make` is optional; the program can also be compiled directly
- `nc`/Netcat is useful for connecting to the server manually

## Build with Make

From this directory, run:

```sh
make
```

This creates the `mini_serv` executable. The Makefile also provides:

```sh
make clean    # remove object files
make fclean   # remove object files and the executable
make re       # clean and rebuild
make run      # rebuild and start on port 8080
make debug    # rebuild with debug symbols and DEBUG messages enabled
make debugrun # build a debug version and start it on port 8080
```

The Makefile is intentionally kept simple because it was written with the
limited time available during the exam. It contains only the basic build,
run, cleanup, and debug targets. There is no automated test target.

The debug targets are useful during development:

- `make debug` adds `-DDEBUG -g` to the compiler flags. This enables the
  optional debug messages and includes debugging symbols in the executable.
- `make debugrun` performs a debug build and starts the server with the
  default `8080` port.

The debug messages are not required for normal use. Build the regular
executable with `make` when they are not needed.

## Build without Make

To compile the program directly, run:

```sh
cc -Wall -Wextra -Werror -o mini_serv mini_serv.c mini_serv_helpers.c
```

The command produces the same `mini_serv` executable as `make`.

## Run the server

The server requires exactly one argument: the TCP port:

```sh
./mini_serv 8080
```

The server stays in the foreground. Stop it with `Ctrl-C`.

In another terminal, connect one or more clients with Netcat:

```sh
nc 127.0.0.1 8080
```

Type a message and press Enter. Each complete line is sent to every other
connected client; the sender does not receive a copy. Open a second terminal
with the same `nc` command to verify the broadcast behavior.

To use another port, pass it to the executable:

```sh
./mini_serv 19090
nc 127.0.0.1 19090
```

The chosen port must be available and should be above 1024 when running
without elevated privileges.

## Limitations

This is a learning and exam-preparation project, not a production-ready
network service. Known limitations include:

- It only listens on the IPv4 loopback address (`127.0.0.1`), so it is not
  intended for remote connections.
- It has no authentication, encryption, access control, persistence, or
  message history.
- It implements a small line-based broadcast protocol, not the full IRC
  protocol and its commands, channels, or user management.
- The server has no configurable client limit or resource-management policy
  beyond the fixed file-descriptor tracking array.
- `send()` errors and several network edge cases are not handled
  comprehensively.
- There is no automated test target in the Makefile; testing is primarily
  manual with Netcat.
- The server accepts one port argument and runs in the foreground. It does
  not provide daemonization, configuration files, or logging.

## Use of AI

AI was used to help create and organize this README, including the usage
instructions, project-purpose description, file-structure explanation, and
limitations section. The README is intended to describe the actual contents
and behavior of this repository; the code and the final documentation remain
the author's responsibility.
