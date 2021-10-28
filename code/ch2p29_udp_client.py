
from socket import socket, AF_INET, SOCK_DGRAM

msgFromClient = "Hello UDP Server"
bytesToSend = str.encode(msgFromClient)

serverAddressPort = ("127.0.0.1", 20001)
bufferSize = 1024

# Create a UDP socket at client side
UDPClientSocket = socket(family=AF_INET, type=SOCK_DGRAM)

# Send to server using created UDP socket
UDPClientSocket.sendto(bytesToSend, serverAddressPort)

msgFromServer = UDPClientSocket.recvfrom(bufferSize)
msg = "Message from Server {}".format(msgFromServer[0])
print(msg)
