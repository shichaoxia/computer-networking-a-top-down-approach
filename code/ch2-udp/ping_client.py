"""Ping client

1. send the ping message using UDP
2. print the response message from server, if any
3. calculate and print the round trip time (RTT)
4. otherwise, print “Request timed out”
5. report the minimum, maximum, average RTTs and packet loss rate.

Message Format: Ping <sequence_number> <time>
"""

import socket
import datetime
import numpy as np

HOST = '127.0.0.1'
PORT = 12000
BUFSIZE = 1024
PKGNUM = 10

packets_received = 0
rtts = []

client_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

for i in range(PKGNUM):
    try:
        send_time = datetime.datetime.now()
        send_time_fmt = send_time.strftime('%H:%M:%S.%f')

        reqt = f'Ping {i+1:02} {send_time_fmt}'
        client_socket.sendto(reqt.encode(), (HOST, PORT))
        print(reqt, end=', ')

        client_socket.settimeout(1.0)

        resp, addr = client_socket.recvfrom(BUFSIZE)
        recv_time = datetime.datetime.now()
        rtt = recv_time - send_time
        rtt_fmt = rtt.microseconds/1000
        print(f'time={rtt_fmt}ms')

        packets_received += 1
        rtts.append(rtt_fmt)
    except socket.timeout:
        print('Request timed out')

print(f'--- {HOST} ping statistics ---')

lose_percent = '{:.1%}'.format((PKGNUM-packets_received)/PKGNUM)
print(f'{PKGNUM} packets transmitted, '
      f'{packets_received} packets received, '
      f'{lose_percent} packet loss')

print(f'round-trip min/avg/max/stddev = '
      f'{np.min(rtts)}'
      f'/{round(np.average(rtts), 3)}'
      f'/{round(np.max(rtts), 3)}'
      f'/{round(np.std(rtts), 3)} ms')
