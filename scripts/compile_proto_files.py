import json

from jinja2 import Template

from titanium_proto import TitaniumFileGenerator

# Define the Jinja2 template for generating the header file
template_str_protobuf_index = """
#ifndef PROTOBUF_INDEX_H
#define PROTOBUF_INDEX_H

#include "stdint.h"

/**
 * @brief Namespace containing constants for different protobufs indexes.
 */
namespace ProtobufIndex {
    /**
    * @brief Invalid index.
    */
    constexpr int8_t INVALID = -1;
{% for memory_area in memory_areas %}
    /**
     * @brief {{ memory_area.description }}
     */
    constexpr int8_t {{ memory_area.name | upper }} = {{ memory_area.index }};
{% endfor %}
    /**
    * @brief The index of the last memory area
    */
    constexpr int8_t LAST_MEMORY_AREA_INDEX = {{ last_index }};

}  // namespace ProtobufIndex

#endif /* PROTOBUF_INDEX_H */
"""

template_str_protobuf_factory = """
#ifndef PROTOBUF_FACTORY_H
#define PROTOBUF_FACTORY_H

#include "esp_log.h"
#include <memory>

#include "ProtobufIndex.h"
{% for include in include_list -%}
#include "{{ include }}Proto.h"
{% endfor %}
class ProtobufFactory {
   public:
    static std::unique_ptr<IProtobuf> CreateProtobuf(uint16_t proto_index) {
        std::unique_ptr<IProtobuf> protobuf = nullptr;

        switch (proto_index) {
{% for memory_area in memory_areas %}
            case ProtobufIndex::{{ memory_area.name | upper }}: {
                protobuf = std::make_unique<{{ memory_area.package_name }}Protobuf>();
                break;
            }
{% endfor %}
            default: {
                ESP_LOGE("ProtobufFactory", "Invalid proto_index: %d", proto_index);
                break;
            }
        }

        return protobuf;
    }
};

#endif /* PROTOBUF_FACTORY_H */
"""

def generate_proto_files(root_dir, memory_area):
    """
    Generates the protobuf files for a given memory area and saves them to the specified directory.
    
    Args:
        root_dir (str): The root directory where files will be generated.
        memory_area (dict): The memory area definition containing protobuf details.
    """
    tp = TitaniumFileGenerator("h")
    tp.import_and_parse_proto_file(raw_data = memory_area)
    tp.generate_header_file(f"{root_dir}/lib/TitaniumOS/Protocols/Protobuf/inc/", True, "Libraries/JSON/ArduinoJson/")
    

def load_firmware_definitions(file_path):
    """
    Loads firmware definitions from a JSON file.
    
    Args:
        file_path (str): Path to the JSON file containing firmware definitions.
    
    Returns:
        dict: A dictionary containing firmware definitions.
    """
    with open(file_path, "r") as file:
        content = json.load(file)
        return content.get("firmware", {})
        

if __name__ == "__main__":
    root_directory = './'
    protobuf_index = {"memory_areas": []}
    last_index = 0
    includes_list = []
    
    firmware_definition = load_firmware_definitions("firmware_definitions.json")
    
    for value in firmware_definition.get("memory_areas", []):
        generate_proto_files(root_directory, value)
        protobuf_index["memory_areas"].append({
            "name": value.get("name"),
            "description": value.get("description"),
            "index": value.get("index"),
            "package_name": value.get("package")
        })
        includes_list.append(value.get("package"))
        last_index += 1
    
    protobuf_index["last_index"] = last_index
    protobuf_index["include_list"] = set(includes_list)
    
    template = Template(template_str_protobuf_index) 
    output_index = template.render(protobuf_index)
    
    with open(f"{root_directory}/lib/TitaniumOS/Protocols/Protobuf/inc/ProtobufIndex.h", "w") as file: # Remove includes
        file.write(output_index)
        
    template = Template(template_str_protobuf_factory) 
    output_factory = template.render(protobuf_index)

    with open(f"{root_directory}/lib/TitaniumOS/Protocols/Protobuf/inc/ProtobufFactory.h", "w") as file: # Remove includes
        file.write(output_factory)