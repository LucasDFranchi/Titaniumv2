import json

from jinja2 import Template, Environment, FileSystemLoader

from .titanium_field import TitaniumField

template_string = """/**
 * @file {{ package_name }}Protobuf.h
 * @brief Auto-generated header file generated.
 */

#ifndef {{ package_name | upper }}_PROTO_H
#define {{ package_name | upper }}_PROTO_H

#include "stdint.h"
#include "string.h"
{%- if proto.json_enable %}
#include "{{ proto.jsmn_path }}jsmn.h"
{%- endif %}
#include "IProtobuf.h"

class {{ package_name }}Protobuf : public IProtobuf {
public:
    {{ package_name }}Protobuf() = default;
    ~{{ package_name }}Protobuf() = default;
{% for field in fields %}
{%- if field.is_array %}
    static constexpr uint16_t {{ field.defined_size }} = {{ field.size }};
{%- endif %}
{%- endfor %}
{% for field in fields %}
{%- if field.is_array %}
    const {{ field.c_type_name }}* Get{{ field.capitalized_name }}(void) const { return this->{{ field.internal_name }}; }
{%- else %}
    {{ field.c_type_name }} Get{{ field.capitalized_name }}(void) const { return this->{{ field.internal_name }}; }
{%- endif %}
{%- endfor %}

    int16_t GetSerializedSize(void) const {
        return ({{ proto.serialized_size }});
    }

    int16_t GetMaxSize(void) const {
        return ({{ proto.maximum_size }});
    }

    static int16_t GetStaticMaxSize(void) {
        return ({{ proto.static_maximum_size }});
    }
{% for field in fields -%}
{%- if field.is_array %}
    int8_t Update{{ field.capitalized_name }}(const {{ field.c_type_name }}* value) {
        if (value == nullptr) {
            return PROTO_INVAL_PTR;
        }

        size_t value_length = strlen(value) + 1;

        if ((value_length == 0) || {{ field.defined_size }} == 0) {
            return PROTO_OVERFLOW;
        }

        if (value_length > {{ field.defined_size }}) {
            return PROTO_INVAL_SIZE;
        }

        memset(this->{{ field.internal_name }}, 0, {{ field.defined_size }});
        memcpy(this->{{ field.internal_name }}, value, value_length);

        return PROTO_NO_ERROR;
    }

    int8_t Update{{ field.capitalized_name }}(const {{ field.c_type_name }}* value, uint16_t string_size) {
        if (value == nullptr) {
            return PROTO_INVAL_PTR;
        }

        if ({{ field.defined_size }} == 0) {
            return PROTO_OVERFLOW;
        }

        if (string_size > {{ field.defined_size }}) {
            return PROTO_INVAL_SIZE;
        }

        memset(this->{{ field.internal_name }}, 0, {{ field.defined_size }});
        memcpy(this->{{ field.internal_name }}, value, string_size);

        return PROTO_NO_ERROR;
    }
{% else %}
    int8_t Update{{ field.capitalized_name }}({{  field.c_type_name }} value) {
        this->{{ field.internal_name }} = value;
        return PROTO_NO_ERROR;
    }
{% endif -%}
{% endfor %}
    int16_t Serialize(char* out_buffer, uint16_t out_buffer_size) const {
        if (out_buffer == nullptr) {
            return 0;
        }

        uint16_t serialized_size = {{ proto.serialized_size }};

        if (out_buffer_size < serialized_size) {
            return 0;
        }
        
{% for member in members %}
    {% if member.type in ['uint8_t', 'uint16_t', 'uint32_t', 'int8_t', 'int16_t', 'int32_t'] %}
        buffer[data_position++] = sizeof({{ member.name }});
        memcpy(&buffer[data_position], &{{ member.name }}, sizeof({{ member.name }}));
        data_position += sizeof({{ member.name }});
    {% elif member.type == 'char*' %}
        uint8_t length = strlen({{ member.name }});
        buffer[data_position++] = length;
        memcpy(&buffer[data_position], {{ member.name }}, length);
        data_position += length;
    {% endif %}
{% endfor %}

        uint16_t offset = 0;
{% for field in fields -%}
{%- if field.is_array %}
        memcpy(&out_buffer[offset], this->{{ field.internal_name }}, strlen(this->{{ field.internal_name }}) + 1);
{%- if not loop.last %}
        offset += strlen(this->{{ field.internal_name }}) + 1;
{%- endif %}
{%- else %}
        memcpy(&out_buffer[offset], &this->{{ field.internal_name }}, sizeof(this->{{ field.internal_name }}));
{%- if not loop.last %}
        offset += sizeof(this->{{ field.internal_name }});
{%- endif %}
{%- endif %}
{%- endfor %}

        return serialized_size;
    }

    int8_t DeSerialize(const char* in_buffer, uint16_t in_buffer_size) {
        if (in_buffer == nullptr) {
            return PROTO_INVAL_PTR;
        }

        uint16_t deserialized_min_size = {{ proto.minimum_size }};

        if (in_buffer_size < deserialized_min_size) {
            return PROTO_INVAL_SIZE;
        }
{% for field in fields -%}
{%- if field.is_array %}
        memset(this->{{ field.internal_name }}, 0, {{ field.defined_size }});
{%- endif %}
{%- endfor %}

{% for member in members %}
    {% if member.type in ['uint8_t', 'uint16_t', 'uint32_t', 'int8_t', 'int16_t', 'int32_t'] %}
        uint8_t size = buffer[data_position++];
        if (size + data_position > size_buffer) return 0;
        memcpy(&{{ member.name }}, &buffer[data_position], size);
        data_position += size;
    {% elif member.type == 'char*' %}
        uint8_t length = buffer[data_position++];
        if (length + data_position > size_buffer) return 0;
        memcpy({{ member.name }}, &buffer[data_position], length);
        {{ member.name }}[length] = '\0'; // Null-terminate
        data_position += length;
    {% endif %}
{% endfor %}

        uint16_t offset = 0;

{%- for field in fields %}
{%- if field.is_array %}
        memcpy(this->{{ field.internal_name }}, &in_buffer[offset], strlen(&in_buffer[offset]) + 1);
{%- if not loop.last %}
        offset += strlen(&in_buffer[offset]) + 1;
{%- endif %}
{%- else %}
        memcpy(&this->{{ field.internal_name }}, &in_buffer[offset], sizeof(this->{{ field.internal_name }}));
{%- if not loop.last %}
        offset += sizeof(this->{{ field.internal_name }});
{%- endif %}
{%- endif %}
{%- endfor %}

        return PROTO_NO_ERROR;
    }
    
{%- if proto.json_enable %}
    int32_t SerializeJson(char* out_buffer, uint16_t out_buffer_size) {
        uint32_t response_length = 0;

        do {
            if (out_buffer == nullptr) {
                break;
            }

            uint16_t serialized_size = {{ proto.serialized_size }};

            if (out_buffer_size < serialized_size) {
                return 0;
            }

            response_length = snprintf(out_buffer, out_buffer_size,
                                       this->_json_string,
{%- for field in fields %}
                                       this->{{ field.internal_name }}{% if not loop.last -%},{%- else %});{% endif -%}
{%- endfor %}
        } while (0);

        return response_length;
    }

    int8_t DeSerializeJson(const char* in_buffer, uint16_t in_buffer_size) {
        auto result = PROTO_NO_ERROR;
        jsmn_parser parser;
        jsmntok_t tokens[this->_NUM_TOKENS];

        jsmn_init(&parser);

        do {
            if (in_buffer == nullptr) {
                result = PROTO_INVAL_PTR;
                break;
            }

            auto num_tokens = jsmn_parse(&parser, in_buffer, strlen(in_buffer), tokens, this->_NUM_TOKENS);

            if (num_tokens != this->_NUM_TOKENS) {
                result = PROTO_INVAL_NUM_TOKEN;
                break;
            }

            jsmntok_t key{};
            jsmntok_t value{};
            uint16_t token_length = 0;

{%- for field in fields %}

            key   = tokens[this->{{ field.internal_name  | upper }}_TOKEN_ID];
            value = tokens[this->{{ field.internal_name  | upper }}_TOKEN_ID + 1];
            token_length = key.end - key.start;

            if (strncmp(in_buffer + key.start, this->{{ field.internal_name | upper }}_TOKEN_NAME, token_length) != 0) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }
{%- if field.is_array %}

            this->Update{{ field.capitalized_name }}(in_buffer + value.start, value.end - value.start);
{%- else %}

            this->Update{{ field.capitalized_name }}(atoi(in_buffer + value.start));
{%- endif %}
{%- endfor %}

            result = PROTO_NO_ERROR;

        } while(0);

        return result;
    }
{%- endif %}

private:
{%- for field in fields %}
    {{ field.c_type_name }} {{ field.internal_name }}{% if field.is_array %}[{{ field.size }}] = {0}{%- else %} = 0{% endif %};
{%- endfor %}

{%- if proto.json_enable %}
    const char* _json_string = R"({
{%- for field in fields %}
    "{{ field.token_name }}": {{ field.format_specifier }}{% if not loop.last -%},{% endif -%}
{%- endfor %}
})";

{%- for field in fields %}  
    const char* {{ field.internal_name | upper }}_TOKEN_NAME = "{{ field.token_name }}";
    const uint8_t {{ field.internal_name  | upper }}_TOKEN_ID = {{ field.token_id }};
{%- endfor %}
    const uint8_t _NUM_TOKENS  = {{ proto.num_tokens }};
};
{%- endif %}
#endif /* {{ package_name | upper }}_PROTO_H */

"""

class TitaniumFileGenerator:
    _SUPPORTED_TYPES = [
        "uint8_t",
        "int8_t",
        "uint16_t",
        "int16_t",
        "uint32_t",
        "int32_t",
        "uint64_t",
        "int64_t",
        "float",
        "double",
        "string",
    ]

    def __init__(self):
        """
        Initializes a new instance of the TitaniumProto class.
        """
        self._content = None
        self._package_name = None
        self._fields = []

        # self._template = Template(template_string)
        self._env = Environment(loader=FileSystemLoader("./scripts/titanium_proto/templates/"))
        self._template = self._env.get_template("protobuf_template.jinja2")

    def _read_file(self, filepath: str):
        """
        Read content from a JSON file.

        Args:
            filepath (str): Path to the Titanium Protobuf JSON file.
        """
        with open(filepath, "r") as file:
            self._content = json.load(file)

    def _update_package_name(self):
        """
        Updates the package name by parsing the protocol file content.

        Raises:
            ValueError: If the package name is missing.
        """
        self._package_name = self._content.get("package")
        if self._package_name is None:
            raise ValueError("Missing Package Name in protocol file.")

    def _parse_fields(self):
        """
        Parse and validate fields from the protocol file content.

        Raises:
            ValueError: If a field has an unsupported data type.
        """
        token_id = 1
        for field in self._content.get("fields"):
            field_type = field.get("type")
            field["token_id"] = token_id
            if field_type not in self._SUPPORTED_TYPES:
                supported_types_str = ", ".join(self._SUPPORTED_TYPES)
                raise ValueError(
                    f"Unsupported data type: {field_type}. Supported types are: {supported_types_str}."
                )

            self._fields.append(TitaniumField(field))
            token_id += 2

    def import_and_parse_proto_file(self, filepath: str = None, raw_data: str = None):
        """
        Imports and parses definitions from a Titanium protobuf file.

        Args:
            filepath (str): Path to the Titanium protobuf file.
        
        Raises:
            ValueError: If there are syntax errors in the protobuf file or required fields are missing.
        """
        if filepath:
            self._read_file(filepath)
        else:
            self._content = raw_data

        self._update_package_name()
        self._parse_fields()
        
    def generate_header_file(self, redirect_outfile: str = "", enable_json: bool = False, jsmn_path: str = ""):
        data = {}
        serialized_size_list = []
        maximum_size_list = []
        minimum_size_list = []
        static_maximum_size_list = []
        num_of_arrays = 0
        
        data["package_name"] = self._package_name
        data["fields"] = []
        for field in self._fields:
            data["fields"].append(field.to_dict())
            
            if field.is_array:
                serialized_size_list.append(f"(strlen(this->{field.internal_name}) + 1)")
                static_maximum_size_list.append(f"{field.defined_size}")
                num_of_arrays += 1
            else:
                serialized_size_list.append(f"sizeof(this->{field.internal_name})")
                static_maximum_size_list.append(f"sizeof({field.type_name})")
                minimum_size_list.append(f"sizeof(this->{field.internal_name})")
                
            maximum_size_list.append(f"sizeof(this->{field.internal_name})")

        data["proto"] = {}            
        data["proto"]["serialized_size"] =  " + ".join(serialized_size_list)
        data["proto"]["maximum_size"] = " + ".join(maximum_size_list)
        data["proto"]["minimum_size"] = " + ".join(minimum_size_list) + f" + {num_of_arrays}"
        data["proto"]["static_maximum_size"] = " + ".join(static_maximum_size_list)
        data["proto"]["num_tokens"] = (len(self._fields) * 2) + 1
        data["proto"]["json_enable"] = enable_json
        data["proto"]["jsmn_path"] = jsmn_path
        data["proto"]["num_var"] = len(self._fields)
        
        rendered_code = self._template.render(data)
        
        with open(f"{redirect_outfile}{self._package_name}Proto.h", 'w') as file:
            file.write(rendered_code)