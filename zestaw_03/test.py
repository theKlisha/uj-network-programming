#!/usr/bin/python3
from socket import socket, AF_INET, SOCK_DGRAM;

tests = [
    (b"0", b"0"),
    (b"1", b"1"),
    (b"32767", b"32767"),
    (b"1+2", b"3"),
    (b"1-2", b"-1"),

    (b"", b"ERROR"),
    (b"-", b"ERROR"),
    (b"-0", b"ERROR"),
    (b"+0", b"ERROR"),
    (b"0-", b"ERROR"),
    (b"0-+0", b"ERROR"),
    (b"0+-0", b"ERROR"),
    (b"\r\n", b"ERROR"),
    (b"\n", b"ERROR"),
    (b"0+\n0", b"ERROR"),
    (b"0+\r\n0", b"ERROR"),
    (b"0+\r\n0", b"ERROR"),
    (b"32767", b"32767"),
]

for (i, (request, expected_response)) in enumerate(tests):
    clientSocket = socket(AF_INET, SOCK_DGRAM)
    clientSocket.settimeout(1)
    addr = ("127.0.0.1", 2020)
    clientSocket.sendto(request, addr)
    response, _ = clientSocket.recvfrom(1024)
    if response != expected_response:
        print("  FAILURE: test {}".format(i))
        print("    SENT:     {}".format(request))
        print("    EXPECTED: {}".format(expected_response))
        print("    GOT:      {}".format(response))
