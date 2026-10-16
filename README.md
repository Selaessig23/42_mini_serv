# 42_mini_serv
 # mini_serv

`mini_serv` is a small TCP chat server. It listens on `127.0.0.1` and
broadcasts each complete line received from one client to all other connected
clients.

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
```

The Makefile is intentionally kept simple because it was written with the
limited time available during the exam. It contains only the basic build,
run, and cleanup targets; debug and test targets are not included.

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