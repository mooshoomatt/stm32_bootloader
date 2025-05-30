file_path = "test.bin"

with open(file_path, "wb") as file:
    data = b""
    for i in range(128):
        data = data + b"\xff"
    
    file.write(data)