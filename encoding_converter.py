import os

def convert_file_encoding(file_path, src_encoding='iso8859-1', dest_encoding='utf-8'):
    with open(file_path, 'r', encoding=src_encoding, errors='replace') as f:
        content = f.read()
    with open(file_path, 'w', encoding=dest_encoding) as f:
        f.write(content)

def convert_directory_encoding(directory, src_encoding='iso8859-1', dest_encoding='utf-8'):
    for root, _, files in os.walk(directory):
        for file in files:
            file_path = os.path.join(root, file)
            try:
                convert_file_encoding(file_path, src_encoding, dest_encoding)
                print(f"Converted: {file_path}")
            except Exception as e:
                print(f"Error converting {file_path}: {e}")

if __name__ == "__main__":
    directory = r"./src/dj"
    convert_directory_encoding(directory)