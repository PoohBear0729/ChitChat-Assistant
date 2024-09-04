import socket
import ollama
import os
from pathlib import Path
import requests
import threading
from datetime import datetime

file_lock = threading.Lock()  # 文件锁


def file_exists(filename):
    return Path(filename).is_file()


def chat_response(content):
    try:
        return ollama.chat(
            model='qwen2_05',
            messages=[{'role': 'user', 'content': content}],
            options={"temperature": 0}
        )
    except Exception as e:
        print(f"聊天请求发生错误: {e}")
        return {'message': {'content': 'Error occurred'}}


import socket
import requests
import os
from datetime import datetime


def send_audio(audio_server: socket.socket, content: str):
    audio_server.listen()
    audio_server.settimeout(60)
    connection, addr = audio_server.accept()
    print(f'与客户端{addr}建立语音连接')
    data = {
        "text": content,
        "text_language": "zh"
    }
    try:
        response = requests.post("http://127.0.0.1:9880", json=data)
        response.raise_for_status()
    except requests.exceptions.RequestException as e:
        print(f"请求发生错误: {e}")
        connection.send(b'error')
        return

    filename = f'response_{datetime.now().strftime("%Y%m%d_%H%M%S")}.wav'
    print("发送语音数据")

    # 将文件内容写入本地文件
    with open(filename, 'wb') as f:
        f.write(response.content)

    file_size = len(response.content)
    print(f'音频文件写入成功, size: {file_size}')

    # 发送文件大小
    connection.send(str(file_size).encode('UTF-8'))
    if connection.recv(4).decode() == "ok":
        # 发送音频数据块
        chunk_size = 512
        for i in range(0, file_size, chunk_size):
            chunk = response.content[i:i + chunk_size]
            connection.send(chunk)
            flag = connection.recv(4).decode('UTF-8')
            if flag != "ok":
                print(f"未收到确认标志: {flag}")
                break

        print('发送成功')
    connection.close()


# 文本服务器
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(("0.0.0.0", 8888))
server.listen()
server.settimeout(60)  # 设置超时
print('文本服务器正在监听...')

# 语音服务器
server_audio = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_audio.bind(("0.0.0.0", 6666))
print('语音服务器正在监听...')

connection, address = server.accept()
print(f'与客户端{address}建立文本连接')

history_dir = './Chat_history/'
os.makedirs(history_dir, exist_ok=True)

while True:
    CurrentDialog: str = connection.recv(64).decode('UTF-8').strip()
    if not CurrentDialog:
        print("文件名接收错误")
        continue
    print("获取对话文件")
    flag = "ok"
    connection.send(flag.encode('UTF-8'))
    print("发送ok回复")
    dialog: str = connection.recv(8196).decode('UTF-8')
    print("收到对话信息")
    print(dialog)

    file_path = os.path.join(history_dir, CurrentDialog)
    with file_lock:
        if file_exists(file_path):
            with open(file_path, 'a+') as history:
                history.seek(0)
                content = history.read()
                content += "\n以上为历史消息，接下来为你可能需要结合上文回答的对话：\n"
                dialog = "用户：" + dialog
                content += dialog
                response = chat_response(content)
                response_text = response.get('message', {}).get('content', '')
                print("发送信息")
                audio_thread = threading.Thread(target=send_audio, args=(server_audio, response_text))
                audio_thread.start()
                print(response_text)
                print("发送信息")
                connection.send(response_text.encode('UTF-8'))
                print("发送信息成功")
                history.write(dialog + "\nllava: " + response_text + "\n")
                audio_thread.join()
                content = ""
        else:
            with open(file_path, 'w') as new_dialog:
                content = "用户：" + dialog
                response = chat_response(content)
                response_text = response.get('message', {}).get('content', '')
                audio_thread = threading.Thread(target=send_audio, args=(server_audio, response_text))
                audio_thread.start()
                print(response_text)
                print("发送信息")
                connection.send(response_text.encode('UTF-8'))
                print("发送信息成功")
                new_dialog.write(content + "\nllava: " + response_text + "\n")
                audio_thread.join()
                content = ""
