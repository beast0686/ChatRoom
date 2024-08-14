import socket
import select
import sys
from threading import Thread

# Create a socket object
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# Check command line arguments
if len(sys.argv) != 3:
    print("Correct usage: script IP_address port_number")
    sys.exit()

IP_address = str(sys.argv[1])
Port = int(sys.argv[2])

# Bind the socket
server.bind((IP_address, Port))
server.listen(100)

list_of_clients = []


def clientthread(conn, addr):
    conn.send(b"Welcome to this chatroom!")
    while True:
        try:
            message = conn.recv(2048)
            if message:
                print(f"<{addr[0]}> {message.decode()}")
                message_to_send = f"<{addr[0]}> {message.decode()}"
                broadcast(message_to_send, conn)
            else:
                remove(conn)
        except:
            continue


def broadcast(message, connection):
    for client in list_of_clients:
        if client != connection:
            try:
                client.send(message.encode())
            except:
                client.close()
                remove(client)


def remove(connection):
    if connection in list_of_clients:
        list_of_clients.remove(connection)


while True:
    conn, addr = server.accept()
    list_of_clients.append(conn)
    print(f"{addr[0]} connected")
    thread = Thread(target=clientthread, args=(conn, addr))
    thread.start()

server.close()
