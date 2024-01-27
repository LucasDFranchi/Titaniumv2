import os
import argparse
import time
import re

"""
Directories present in these list could only be validated in the real hardware instead of emulation
"""
skip_hardware_test = ["test_gpio"]


def handle_argparse():
    """
    Parse command-line arguments.

    Returns:
        argparse.Namespace: Parsed command-line arguments.
    """
    parser = argparse.ArgumentParser(
        description="Script used to run tests in qemu emulated environment"
    )
    parser.add_argument(
        "--env",
        "-e",
        type=str,
        help="The platformio defined environment.",
        required=True,
    )
    
    parser.add_argument(
        "--filter",
        "-f",
        type=str,
        help="Filter the test folder.",
        required=False,
    )

    return parser.parse_args()


def get_tests_folders():
    """
    Retrieve subfolders in the './test' directory that start with 'test'.

    Returns:
        list: List of test subfolders.
    """
    try:
        items = os.listdir("./test")
        sub_folders = [
            item for item in items if os.path.isdir(os.path.join("./test", item))
        ]

        return [folder for folder in sub_folders if folder.startswith("test")]
    except Exception as e:
        print(f"An error occurred: {e}")
        return []


def build_test_firmware(env: str, test: str):
    """
    Build test firmware for a specific environment and test folder.

    Args:
        env (str): The platformio environment.
        test (str): The test folder.
    """
    build_command = f"pio test -e {env} --without-uploading --without-testing -f {test} > /dev/null"
    os.system(build_command)


def merge_bin_image(
    env: str,
    flash_size: int = 8,
    bootloader_addr: int = 0x1000,
    partitions_addr: int = 0x8000,
    fw_addr: int = 0x10000,
):
    """
    Merge binary image for the specified environment using esptool.

    Args:
        env (str): The platformio environment.
        flash_size (int, optional): Flash size in MB. Defaults to 8.
        bootloader_addr (int, optional): Address for the bootloader. Defaults to 0x1000.
        partitions_addr (int, optional): Address for the partitions. Defaults to 0x8000.
        fw_addr (int, optional): Address for the firmware. Defaults to 0x10000.
    """
    merge_command = f"""
    esptool.py --chip esp32 merge_bin --output result.bin --fill-flash-size {flash_size}MB \
    {hex(bootloader_addr)}  .pio/build/{env}/bootloader.bin \
    {hex(partitions_addr)}  .pio/build/{env}/partitions.bin \
    {hex(fw_addr)} .pio/build/{env}/firmware.bin \
    --flash_mode dio --flash_freq 40m
    """
    os.system(merge_command)

def run_test(env: str, error_list: list[str]):
    """
    Run test in qemu emulated environment.

    Args:
        env (str): The platformio environment.
    """
    exec_command = (
        "qemu-system-xtensa -machine esp32 -drive file=result.bin,if=mtd,format=raw "
        "-serial file:output.log -nographic -monitor unix:/tmp/qemu-monitor,server,nowait & "
    )
    os.system(exec_command)
    time.sleep(5)
    monitor_log_until_completion("output.log", "main_task: Returned from app_main()")
    os.system('echo "quit" | socat - UNIX-CONNECT:/tmp/qemu-monitor &')

    parse_output(env, error_list)
    
def monitor_log_until_completion(log_file, completion_line):
    print("Monitoring log file for completion...")
    while True:
        with open(log_file, 'r') as file:
            if completion_line in file.read():
                print("Completion line found: {}".format(completion_line))
                break
        time.sleep(1)
        
def parse_output(env: str, error_list: list[str]):
    pattern = r'[0-9]+ Tests [0-9]+ Failures [0-9]+ Ignored'

    with open('output.log', 'r') as file:
        for line in file:
            if re.search(pattern, line):
                parse_command = f"ruby .pio/libdeps/{env}/Unity/auto/parse_output.rb -xml output.log"
                os.system(parse_command)
                return
            
            if "FAIL:" in line:
                error_list.append(line)

    print("Pattern not found in log file.")
    
def print_failures(error_list: list[str]):
    print("=================== FAILURES =====================")
    for error_line in error_list:
        print(error_line)
        
    if len(error_list) > 0:
        exit(-1)
        
def clear_test_artifactories():
    os.system("rm output.log")
    os.system("rm report.xml")
    os.system("rm result.bin")
    

def main():
    """Main function."""
    parser = handle_argparse()
    error_list = []

    platformio_environment, test_filter = parser.env, parser.filter
    test_directory = get_tests_folders()

    if test_filter is None:
        for test_folder in test_directory:
            if test_folder in skip_hardware_test:
                continue
            build_test_firmware(platformio_environment, test_folder)
            merge_bin_image(platformio_environment)
            run_test(platformio_environment, error_list)
    else:
        build_test_firmware(platformio_environment, test_filter)
        merge_bin_image(platformio_environment)
        run_test(platformio_environment, error_list)
        
    clear_test_artifactories()        
    print_failures(error_list)


if __name__ == "__main__":
    main()
