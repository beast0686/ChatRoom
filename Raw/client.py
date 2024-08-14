import socket
import select
import sys

# Create a socket object
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Check command line arguments
if len(sys.argv) != 3:
    print("Correct usage: script IP_address port_number")
    sys.exit()

IP_address = str(sys.argv[1])
Port = int(sys.argv[2])

# Connect to the server
server.connect((IP_address, Port))

while True:
    sockets_list = [sys.stdin, server]
    read_sockets, _, _ = select.select(sockets_list, [], [])

    for socks in read_sockets:
        if socks == server:
            message = socks.recv(2048)
            print(message.decode())
        else:
            message = sys.stdin.readline()
            server.send(message.encode())
            sys.stdout.write("<You> ")
            sys.stdout.write(message)
            sys.stdout.flush()

server.close()
