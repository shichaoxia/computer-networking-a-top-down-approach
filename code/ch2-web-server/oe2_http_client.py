"""HTTP client

Instead of using a browser, write your own HTTP client to test your server.

Your client will connect to the server using a TCP connection, send an HTTP
request to the server, and display the server response as an output.
"""

import argparse
import socket

BUFSIZE = 2048

parser = argparse.ArgumentParser()
parser.add_argument('host')
parser.add_argument('port', type=int)
parser.add_argument('filename')
args = parser.parse_args()

clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
clientSocket.connect((args.host, args.port))

req = (f'GET /{args.filename} HTTP/1.1\n'
       f'Host: {args.host}:{args.port}\n'
       '\n')

clientSocket.send(req.encode())

resp = clientSocket.recv(BUFSIZE)

while resp:
    print(resp.decode(), end='')
    resp = clientSocket.recv(BUFSIZE)

clientSocket.close()
