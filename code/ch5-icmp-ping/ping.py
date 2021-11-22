#!/usr/bin/env python3
"""[summary]"""

# pylint: disable=global-statement, invalid-name

import argparse
import binascii
import datetime
import os
import select
import struct
import sys
import time
from socket import (AF_INET, SOCK_RAW, gethostbyname, getprotobyname, htons,
                    socket)

ICMP_ECHO_REQUEST = 8

verbose = False

def checksum(astr: bytes) -> int:
    """Need no concern.

    See `Calculating checksum for ICMP echo request in Python`_.

    .. _Calculating checksum for ICMP echo request in Python:
       https://stackoverflow.com/a/55222144
    """
    csum = 0
    count_to = (len(astr) / 2) * 2  # 先处理前面的偶数个8位

    count = 0
    while count < count_to:
        # Adjacent octets in the source_string are paired to form
        # 16-bit integers
        # No need ord in python3
        # https://stackoverflow.com/a/50111388
        this_val = (astr[count+1] << 8) + astr[count]
        csum = csum + this_val
        csum = csum & 0xffffffff  # truncate the sum to 32-bits
        count = count + 2

    # In case of odd number of octets, add the remaining octet to the
    # sum.
    if count_to < len(astr):
        csum = csum + astr[len(astr) - 1]
        csum = csum & 0xffffffff

    # 高16位与低16位相加
    csum = (csum >> 16) + (csum & 0xffff)
    # 把高位溢出加回低位，还是做1's complement sum
    csum += csum >> 16
    answer = ~csum
    answer = answer & 0xffff
    answer = answer >> 8 | (answer << 8 & 0xff00)
    return answer


def receive_one_ping(my_socket: socket, the_id: int, timeout: int, dest_addr: str) -> None:
    """Receive the structure ICMP_ECHO_REPLY and fetch the information."""

    # Python的select()函数是部署底层操作系统的直接接口。它监视着套
    # 接字、打开的文件和管道直到它们变得可读可写或有错误发生。
    # https://learnku.com/docs/pymotw/select-wait-for-io-efficiently/3429#5dd208
    readable, _, _ = select.select([my_socket], [], [], timeout)

    if readable == []:  # Timeout
        print("Request timed out.")
        return

    time_received = time.time()
    rec_packet, addr = my_socket.recvfrom(1024)
    dt = datetime.datetime.fromtimestamp(time_received)
    dt = dt.strftime('%H:%M.%f')
    vprint(f'Received at {dt}\n  from: {addr}\n  packet: {binascii.hexlify(rec_packet)}')

    #Fill in start
    # Fetch the ICMP header from the IP packet
    icmp_content = rec_packet[20:]
    _, _, _, _, _, data = struct.unpack('bbHHhd', icmp_content)
    print(f'time={(time_received-data)*1000:.3f} ms')
    #Fill in end


def send_one_ping(my_socket: socket, dest_addr: str, the_id: int) -> None:
    """send_one_ping"""
    # Header is type (8), code (8), checksum (16), id (16), sequence (16)

    my_checksum = 0
    # Make a dummy header with a 0 checksum.
    # struct -- Interpret strings as packed binary data
    header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, my_checksum, the_id, 1)
    data = struct.pack("d", time.time())
    # Calculate the checksum on the data and the dummy header.
    my_checksum = checksum(header + data)

    # Get the right checksum, and put in the header
    if sys.platform == 'darwin':
        my_checksum = htons(my_checksum) & 0xffff
        # Convert 16-bit integers from host to network byte order.
    else:
        my_checksum = htons(my_checksum)

    vprint(f'Checksum: {my_checksum:x}')
    header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, my_checksum, the_id, 1)
    packet = header + data

    vprint(f'Send packet: {binascii.hexlify(packet)}')

    # AF_INET address must be tuple, not str
    my_socket.sendto(packet, (dest_addr, 1))
    # Both LISTS and TUPLES consist of a number of
    # which can be referenced by their position number within the object


def do_one_ping(dest_addr: str, timeout: int) -> None:
    """do_one_ping"""
    icmp = getprotobyname("icmp")
    # SOCK_RAW is a powerful socket type. For more details see:
    # http://sock-raw.org/papers/sock_raw

    #Fill in start
    # Create Socket here
    my_socket: socket = socket(family=AF_INET, type=SOCK_RAW, proto=icmp)
    #Fill in end

    # Return the current process i
    # 0xFFFF取低16位
    my_id = os.getpid() & 0xFFFF
    send_one_ping(my_socket, dest_addr, my_id)
    receive_one_ping(my_socket, my_id, timeout, dest_addr)

    my_socket.close()


def ping(host: str, timeout: int = 1) -> None:
    """Ping host with timeout."""
    # timeout=1 means: If one second goes by without a reply from the
    # server, the client assumes that either the client’s ping or the
    # server’s pong is lost
    dest = gethostbyname(host)
    print(f'({dest}):')

    # Send ping requests to a server separated by approximately one
    # second
    while 1:
        do_one_ping(dest, timeout)
        time.sleep(1)  # one second

def vprint(s: str) -> None:
    """Print only verbose"""
    if verbose:
        print(s)
    else:
        pass

def main() -> None:
    """main"""
    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument("host")
    parser.add_argument("-v", "--verbose", action="store_true")
    args = parser.parse_args()
    global verbose
    verbose = args.verbose
    print(f'Ping {args.host}', end=' ')
    ping(args.host)


if __name__ == "__main__":
    main()
