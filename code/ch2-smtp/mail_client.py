"""SMTP client with SSL"""

import base64
import socket
import ssl
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('account', help='base64编码的账户和密码，echo -ne "\00user@gmail.com\00password" | base64')
parser.add_argument('mail_from', help='发件人')
parser.add_argument('to', help='收件人')
parser.add_argument('msg', help='内容')
parser.add_argument('pic', help='图片')
args = parser.parse_args()

ACCOUNT = args.account
BUF_SIZE = 1024
FROM = f'<{args.mail_from}>'
TO = f'<{args.to}>'
MSG = f'{args.msg}\r\n\r\n'
END_MSG = '\r\n.\r\n'
PIC = args.pic

MAIL_SERVER = 'smtp.gmail.com'
PORT = 465

with open(PIC, 'rb') as f:
    img = base64.b64encode(f.read())

sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
client_socket = ssl.wrap_socket(sock)
client_socket.connect((MAIL_SERVER, PORT))

print(f'<- {client_socket.recv(BUF_SIZE)}')

# Send HELO command and print server response.
helo_command = 'HELO Alice\r\n'.encode()
print(f'---> {helo_command}')
client_socket.send(helo_command)
print(f'<- {client_socket.recv(BUF_SIZE)}')

# Send AUTH command and print server response.
auth_command = f'AUTH PLAIN {ACCOUNT}\r\n'.encode()
print(f'---> {auth_command}')
client_socket.send(auth_command)
print(f'<- {client_socket.recv(BUF_SIZE)}')

# Send MAIL FROM command and print server response.
mail_from_command = f'MAIL FROM: {FROM}\r\n'.encode()
print(f'---> {mail_from_command}')
client_socket.send(mail_from_command)
print(f'<- {client_socket.recv(BUF_SIZE)}')

# Send RCPT TO command and print server response.
rcpt_command = f'RCPT TO: {TO}\r\n'.encode()
print(f'---> {rcpt_command}')
client_socket.send(rcpt_command)
print(f'<- {client_socket.recv(BUF_SIZE)}')

# Send DATA command and print server response.
data_command = 'DATA\r\n'.encode()
print(f'---> {data_command}')
client_socket.send(data_command)
print(f'<- {client_socket.recv(BUF_SIZE)}')

# Send message
mime_header = ('MIME-Version: 1.0\r\n'
               'Content-Type: multipart/mixed; '
               'boundary="simple boundary\r\n\r\n').encode()
print(f'---> {mime_header}')
client_socket.send(mime_header)

# Send text
text_mime = ('--simple boundary\r\n'
             'Content-Type: text/plain; charset=utf-8\r\n'
             'Content-Transfer-Encoding: quoted-printable\r\n'
             'Content-Disposition: inline\r\n\r\n').encode()
print(f'---> {text_mime}')
client_socket.send(text_mime)

print(f'---> {MSG.encode()}')
client_socket.send(MSG.encode())

# Send image
img_mime = ('--simple boundary\r\n'
            'Content-Type: image/jpeg\r\n'
            'Content-Transfer-Encoding: BASE64\r\n'
            'Content-Disposition: inline\r\n\r\n').encode()
print(f'---> {img_mime}')
client_socket.send(img_mime)

print(f'---> {img}')
client_socket.send(img)

end_boundary = '\r\n--simple boundary--'.encode()
print(f'---> {end_boundary}')
client_socket.send(end_boundary)

# Message ends with a single period.
print(f'---> {END_MSG.encode()}')
client_socket.send(END_MSG.encode())

print(f'<- {client_socket.recv(BUF_SIZE)}')

# Send QUIT command and get server response.
quit_command = 'QUIT\r\n'.encode()
print(f'---> {quit_command}')
client_socket.send(quit_command)
print(f'<- {client_socket.recv(BUF_SIZE)}')

client_socket.close()
