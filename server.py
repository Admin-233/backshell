import socket
import threading
import time

server_host = "127.0.0.1"#换成你的服务器ip
server_port = 233

deviceslist = []

def handle_client(client_socket):
    isdevice = False
    # 处理客户端连接的逻辑
    for i in deviceslist:
        if i[0] == client_socket:
            isdevice = True
            devicename = i[1]
        else:isdevice=False
        
    if isdevice == True:
        while 1:
            time.sleep(1)
            try:
                client_socket.send(b"test")
            except (ConnectionResetError, ConnectionAbortedError, OSError):
                deviceslist.remove((client_socket,devicename))
                print("trytoclose")
                client_socket.close()
                break
    else:
        print("admin connect")
        
        while 1:
            try:
                try:
                    sendto = int(client_socket.recv(1024).decode())
                except ValueError:
                    client_socket.send(b"not a number, what do you want to do")
                    continue
                    
                print(sendto,sendto==999)
                msg = ""
                
                if sendto==999:#查询设备列表
                
                    if len(deviceslist) == 0:#没有设备
                        client_socket.send(b"nodevices")
                        continue
                    
                    index=0
                    for i in deviceslist:
                        msg+=(str(index) + "\t" + i[1] + "\n")
                        index+=1
                    print("sendmsg\n"+msg)
                    client_socket.send(msg.encode())
                    continue
                
                print("is out of range",sendto > len(deviceslist))
                if sendto+1 > len(deviceslist):#选择设备超出范围
                    print("out of range")
                    client_socket.send(b"out of range")
                    continue
                   
                client_socket.send(("ok select device "+deviceslist[sendto][1]).encode())
                
                command = client_socket.recv(1024).decode()
                print("command",command)
 
            except socket.error:#管理员断开
                print("admin disconnected")
                break
            
            print("here run command")
            print(command.encode())
            deviceslist[sendto][0].send(command.encode())
            
            sendback = deviceslist[sendto][0].recv(65535)#客户端返回信息
            print("sendback",sendback)
            
            try:
                client_socket.send(sendback)
            except socket.error:#管理员断开
                print("admin disconnected")
                break

# 创建Socket对象
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 绑定到指定的地址和端口
while 1: # 绑定失败则重试
    try:
        server_socket.bind((server_host, server_port))
        break
    except OSError:
        time.sleep(5)
        pass

# 开始监听客户端连接
server_socket.listen(5)
print(f"Server listening on {server_host}:{server_port}")

while True:
    # 接受客户端连接
    client_socket, client_addr = server_socket.accept()
    print(f"Accepted connection from {client_addr[0]}:{client_addr[1]}")
    print(len(deviceslist))

    try:
        auth = client_socket.recv(1024)
    except ConnectionResetError:
        client_socket.close()
        continue
    print(auth)
    if auth.decode() != "admin":
        deviceslist.append((client_socket, auth.decode()))
    else:pass
    
    
    # 创建一个新的线程来处理客户端连接
    client_thread = threading.Thread(target=handle_client, args=(client_socket,))
    client_thread.start()
