"""UDP heartbeat server

The client sends a sequence number and current timestamp in the UDP packet to
the server, which is listening for the Heartbeat of the client.

Upon receiving the packets, the server calculates the time difference and
reports any lost packets. If the Heartbeat packets are missing for some
specified period of time, we can assume that the client application has stopped.

Heartbeat message format: <seq_num> <timestamp>
"""

import random
import datetime
import socket

serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSocket.bind(('', 12000))
print('Ready to serve...')

last_seq = None
recv_time = None

while True:
    try:
        # 2.5秒收不到消息就判定为下线
        serverSocket.settimeout(2.5)

        message, address = serverSocket.recvfrom(1024)
        seq, *send_time = message.split()

        seq = int(seq.decode())
        if last_seq != None and seq - last_seq != 1:
            for i in range(last_seq+1, seq):
                print(f'hb_seq={i} lost')
        last_seq = seq

        send_time = datetime.datetime.fromisoformat(' '.join(map((lambda x: x.decode()), send_time)))
        recv_time = datetime.datetime.now()
        trans_time = (recv_time-send_time).microseconds/1000
        print(f'hb_seq={seq} time={trans_time}ms')

        serverSocket.sendto('ok'.encode(), address)
    except socket.timeout:
        if recv_time != None:
            print('Client stopped')
            recv_time = None
