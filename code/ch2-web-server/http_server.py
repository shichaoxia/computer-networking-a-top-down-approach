"""Web server

Put an HTML file in the same directory that the server is in. Then try to get a
file that is not present at the server. You should get a “404 Not Found”
message.
"""

from socket import *

HOST = '127.0.0.1'
PORT = 12000
BUFSIZE = 1024

serverSocket = socket(AF_INET, SOCK_STREAM)
serverSocket.bind((HOST, PORT))
serverSocket.listen()

while True:
    print('--> Ready to serve...')
    connectionSocket, addr = serverSocket.accept()

    try:
        message = connectionSocket.recv(BUFSIZE).decode()
        print(f'--> Receive a request:\n{message}')

        # GET /hello.html HTTP/1.1
        filename = message.split()[1]  # /hello.html
        f = open(filename[1:])  # hello.html
        outputdata = f.read()
        print(f'--> Found local file:\n{outputdata}')
        f.close()

        connectionSocket.send('HTTP/1.1 200 OK'.encode())
        connectionSocket.send('\n'.encode())

        for i in range(0, len(outputdata)):
            connectionSocket.send(outputdata[i].encode())

        print('--> Sent the file')
        connectionSocket.close()

    except IOError:
        print('--> Local file does not exist')
        resp = ('HTTP/1.1 404 Not Found\n'
                '\n'
                'Try to get a file that is not present at the server.')
        connectionSocket.send(resp.encode())
        connectionSocket.close()

serverSocket.close()
