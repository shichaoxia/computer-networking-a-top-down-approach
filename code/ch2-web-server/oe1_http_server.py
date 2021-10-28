"""Multithreaded server

Implement a multithreaded server that is capable of serving multiple requests
simultaneously.
"""

from socket import *
import threading

HOST = ''
PORT = 12000
BUFSIZE = 1024

def listen_to(connectionSocket, address):
    try:
        message = connectionSocket.recv(BUFSIZE).decode()
        print(f'--> Receive a request:\n{message}')

        filename = message.split()[1]
        f = open(filename[1:])
        outputdata = f.read()
        f.close()

        connectionSocket.send('HTTP/1.1 200 OK'.encode())
        connectionSocket.send('\n'.encode())

        for i in range(0, len(outputdata)):
            connectionSocket.send(outputdata[i].encode())

        print('--> Sent the file')
        connectionSocket.close()

    except IOError:
        print('--> File does not exist')
        resp = ('HTTP/1.1 404 Not Found\n'
                '\n'
                'Try to get a file that is not present at the server.')
        connectionSocket.send(resp.encode())
        connectionSocket.close()

serverSocket = socket(AF_INET, SOCK_STREAM)
serverSocket.bind((HOST, PORT))
serverSocket.listen()

while True:
    print('--> Ready to serve...')
    connectionSocket, addr = serverSocket.accept()
    print(f'--> Connected to: {addr}')

    threading.Thread(target=listen_to, args=(connectionSocket, addr)).start()

serverSocket.close()
