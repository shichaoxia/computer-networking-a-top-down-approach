"""UDP heartbeat client
"""

import socket
import datetime
import random
import time

HOST = '127.0.0.1'
PORT = 12000
BUFSIZE = 1024

seq = 0

client_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

while True:
    seq += 1
    r = random.random()
    if r <= 0.3:
        # 模拟丢包
        print(f'{seq} 模拟丢包')
    elif r > 0.3 and r <= 0.4:
        # 模拟下线
        print(f'{seq} 模拟下线，睡眠4秒')
        time.sleep(4)
        seq = 0
    else:
        msg = f'{seq} {datetime.datetime.now()}'
        client_socket.sendto(msg.encode(), (HOST, PORT))
        print(f'{msg}', end=' ')
        resp, addr = client_socket.recvfrom(BUFSIZE)
        print(f'{resp.decode()}')
        time.sleep(1*r)

client_socket.close()
