# 42_mini_serv
`mini_serv` is a minimalistic TCP chat server. It listens on `127.0.0.1` and
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

## What this project focuses on

The central subject of this repository is the smallest useful event-driven
server loop:

- create, bind, and listen on a TCP socket;
- accept clients and assign them an identifier;
- monitor the listening socket and clients with `select()`;
- accumulate TCP fragments until a newline-terminated message is complete;
- broadcast each complete line to the other connected clients; and
- close sockets and release buffers when clients leave or the server stops.

`42_mini_serv` is an exam-oriented exercise in understanding the descriptor loop and stream buffering with as little surrounding machinery as possible.

## File structure

- `mini_serv.c` is the main program with the main server loop. It creates and binds the listening socket, tracks connected clients, calls `select()`, receives data, and
  broadcasts complete messages.
- `mini_serv_helpers.c` contains the two string-buffer helpers used by the
  server. They handle joining received fragments and extracting complete
  newline-terminated messages. These functions were provided by the subject, for that reason they are kept separate from the main server loop.
- `debug.h` contains the optional signal-handling declaration and the
  `DEBUG_PRINT` macro used by the debug Make targets.
- `main_helper.c` is a separate example of the pre-given socket setup. It is
  kept as a reference and is not compiled by the Makefile.

This split keeps the exam-oriented server logic easy to read while isolating
the provided helper functions and optional development code. The Makefile
only compiles `mini_serv.c` and `mini_serv_helpers.c`. Normally, the main file mini_serv.c would have been devided into several logical inherent files, but the exam subject requires to just upload one single file. 

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
network service. The following limits are intentional consequences of its
small scope:

- It only listens on the IPv4 loopback address (`127.0.0.1`), so it is not
  intended for remote connections.
- It has no authentication, encryption, access control, persistence, message
  history, private messaging, channels, or configurable server settings.
- It implements only a newline-delimited broadcast protocol. It is not an IRC
  implementation and does not provide IRC commands, registration, nicknames,
  channel membership, or user management.
- The event loop uses `select()`, so its usable descriptor range is restricted
  by the platform's `FD_SETSIZE`. Accepted sockets whose descriptors cannot be
  safely represented by `select()` or indexed in the server's client arrays
  are rejected. This is a descriptor-safety limit, not a guarantee that
  `MAX_CLIENTS` (`65536`) clients can connect.
- The listening backlog is fixed at `10` (according to pre-give main). 
  It is a kernel-managed pending-connection queue, so the operating system may 
  cap its effective size. The server accepts one pending connection per 
  event-loop pass; connection bursts may therefore remain queued temporarily 
  or be rejected once the kernel queue is full.
- Input is read in chunks of at most `1023` bytes. The server correctly keeps
  incomplete lines between reads, but a client that never sends a newline can
  make its heap-backed input buffer grow without a configured maximum.
- Outgoing data is sent directly with `send()` on blocking sockets. Partial
  writes, back-pressure, and `send()` errors are not handled comprehensively,
  so a slow recipient can temporarily block the server and slow or
  disconnected recipients can cause messages to be lost. There is no
  per-client output queue.
- Port arguments are validated as decimal values in the valid TCP port range
  (`1`-`65535`); malformed values, trailing characters, and out-of-range
  values are rejected.
- Several network edge cases, such as nonblocking I/O, complete output-queue
  management, and fine-grained protocol validation, are outside the
  exercise's scope.
- There is no automated test target in the Makefile; testing is primarily
  manual with Netcat. The debug targets provide diagnostics, but they are not
  a substitute for a full integration or stress test suite.
- The server accepts one port argument and runs in the foreground. It does
  not provide daemonization, configuration files, or logging.

## Use of AI

AI was used as assistance to create this README, including the usage
instructions, project-purpose description, file-structure explanation, and
limitations section. Code and in-code documentation was kept untouched by AI.
