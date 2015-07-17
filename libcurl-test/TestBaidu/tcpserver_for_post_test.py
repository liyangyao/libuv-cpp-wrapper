from socket import *
from time import ctime

HOST = ''
PORT = 80
BUFSIZ = 1024
ADDR = (HOST, PORT)

tcpSerSock = socket(AF_INET, SOCK_STREAM)
tcpSerSock.bind(ADDR)
tcpSerSock.listen(5)

while True:
    print 'waiting for connection...'
    tcpCliSock, addr = tcpSerSock.accept()
    print '...connected from:', addr

    while True:
        data = tcpCliSock.recv(BUFSIZ)
        if not data:
            break
        print data
        tcpCliSock.send('HTTP/1.1 200 OK\r\nContent-Length: 6\r\n\r\nabcdef]')
    tcpCliSock.close()
    print 'closed'

tcpSerSock.close()
