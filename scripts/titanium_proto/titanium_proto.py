import argparse

from .src import TitaniumFileGenerator

def handle_arguments():
    """
    Parse command-line arguments.

    Returns:
        argparse.Namespace: An object containing the parsed arguments.
    """
    parser = argparse.ArgumentParser(
        description="This script runs inventory and collects data from Titanium protobuf files."
    )

    parser.add_argument(
        "--file_path", "-fp", 
        help="Path to the Titanium Protobuf file to be processed. Provide a single file path.",
        required=False
    )
    
    parser.add_argument(
        "--raw_data", "-r", 
        help="Json as raw data",
        required=False
    )
    
    parser.add_argument(
        "--output_path", "-o", 
        help="Directory path where the generated header file will be saved.",
        required=False,
        default="",
    )

    parser.add_argument(
        "--enable_json",
        help="Add JSON serialization and deserialization.",
        action="store_true"
    )
    
    parser.add_argument(
        "--jsmn_path",
        help="Add JSON serialization and deserialization.",
        required=False,
        default="",
    )

    return parser.parse_args()

def main():
    args = handle_arguments()
    tp = TitaniumFileGenerator()
    
    tp.import_and_parse_proto_file(args.file_path, args.raw_data)
    tp.generate_header_file(args.output_path, args.enable_json, args.jsmn_path)


if __name__ == "__main__":
    main()
    