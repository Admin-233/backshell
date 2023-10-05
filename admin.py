import socket
import time

def main():
    server_host = "127.0.0.1"#换成你的服务器ip
    server_port = 233
    client_name = "admin"  # 发送的验证信息

    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    client_socket.connect((server_host, server_port))
    print("Connected to the server")
    
    client_socket.send("admin".encode())#auth

    time.sleep(0.5)

    selectdevice = ""
    commandinput = ""
    while 1:
        if selectdevice == "":#选择设备
            
            client_socket.send("999".encode())

            print(client_socket.recv(1024).decode())

            while 1:
                selectdevice = input("enter device index")
                if selectdevice == "":break
                try:
                    int(selectdevice)
                    break
                except ValueError:
                    print("check what you type in")
                    continue
            
            if selectdevice == "":continue

        client_socket.send(selectdevice.encode())

        selectback = client_socket.recv(1024).decode()
        print(selectback)
        if selectback == "out of range":
            selectdevice = ""
            continue

        commandinput = input("input command")

        if commandinput == "":
            print("not allow empty command")
            client_socket.send(b"asdf")#随便发点什么跳过这一次
            print(client_socket.recv(65535).decode("GBK"))
            print("check1")
            continue

        client_socket.send(commandinput.encode())
        if commandinput == "stop" or commandinput == "exitdevice":
            print("stopcheck")
            print(client_socket.recv(65535).decode("GBK"))
            selectdevice = ""
            continue

        print(client_socket.recv(65535).decode("GBK"))


if __name__ == "__main__":
    main()
