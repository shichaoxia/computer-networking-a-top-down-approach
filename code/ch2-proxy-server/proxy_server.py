"""HTTP Web Proxy Server
"""

import socket
import sys
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('server_ip', help='IP address of proxy server')
parser.add_argument('port', type=int, help='Port of proxy server')
args = parser.parse_args()

print(f'>>> args: {args}')

HOST = args.server_ip
PORT = args.port
BUFSIZE = 1024

tcp_ser_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
tcp_ser_sock.bind((HOST, PORT))
tcp_ser_sock.listen()

while True:
    print('>>> Ready to serve...')
    tcp_cli_sock, addr = tcp_ser_sock.accept()
    print(f'>>> Received a connection from: {addr}')

    message = tcp_cli_sock.recv(BUFSIZE).decode()
    print(f'>>> Received request message:\n{message}')

    # GET /www.dianping.com HTTP/1.1
    path = message.split()[1]  # /www.dianping.com
    print(f'>>> path: {path}')
    rel_path = path.partition("/")[2]  # www.dianping.com
    print(f'>>> rel_path: {rel_path}')

    try:
        print('>>> Try to read from cache file')
        f = open(rel_path, 'r')
        file_content = f.readlines()
        print(f'>>> Cache File content:\n{file_content[0:10]}...')

        send_content = list(map(lambda l: l.replace('\n', '\r\n'), file_content))

        for l in send_content:
            tcp_cli_sock.send(l.encode())

        print('Read from cache')
    except IOError:
        print('>>> Failed to read cache file')
        print('>>> Try to connect origin host')
        c = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
        hostn = rel_path.replace('www.', '', 1)
        print(f'>>> hostn: {hostn}')

        c.connect((hostn, 80))

        rqst = ('GET / HTTP/1.0\r\n'
                '\r\n').encode()
        print(f'Send request:\n{rqst}')
        c.send(rqst)

        resp = b''
        while True:
            part = c.recv(BUFSIZE)
            resp += part
            if len(part) == 0:
                # end of data
                break

        print(f'>>> resp: {resp[0:10]}...')
        c.close()

        tcp_cli_sock.send(resp)

        cache_path = f'./{rel_path}'
        print(f'>>> Cache response to {cache_path}')
        with open(cache_path, 'wb') as f:
            f.write(resp)

    tcp_cli_sock.close()

tcp_ser_sock.close()
