import os

def print_proto_files(root_dir):
    file_list = []
    for root, dirs, files in os.walk(root_dir):
        if 'proto' in dirs:
            proto_dir = os.path.join(root, 'proto')
            for file_name in os.listdir(proto_dir):
                file_list.append(file_name)
                os.system(f"titanium-proto -fp {proto_dir}/{file_name} -o {proto_dir}/../inc/ --enable_json")
                
    return file_list

# Example usage:
if __name__ == "__main__":
    root_directory = './'
    print_proto_files(root_directory)
