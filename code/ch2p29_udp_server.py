
from socket import socket, AF_INET, SOCK_DGRAM

localIP = "127.0.0.1"
localPort = 20001
bufferSize = 1024

msgFromServer = "Hello UDP Client"
bytesToSend = str.encode(msgFromServer)

# Create a datagram socket
UDPServerSocket = socket(family=AF_INET, type=SOCK_DGRAM)

# Bind to address and ip
UDPServerSocket.bind((localIP, localPort))

print("UDP server up and listening")

# Listen for incoming datagrams
while(True):
    message, address = UDPServerSocket.recvfrom(bufferSize)
    print("Message from Client: {}".format(message))
    print("Client IP Address: {}".format(address))
    # Sending a reply to client
    UDPServerSocket.sendto(bytesToSend, address)
