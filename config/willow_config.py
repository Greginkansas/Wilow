import json
import sys

namespace = "willow"

# Determine type for NVS
def type_condition(v):
    if type(v) is str:
        return "string"
    elif type(v) is int:
        return "u16"
    elif type(v) is bool:
        return "string"
    else:
        print(f'{v} type is not str, int, or bool')
        sys.exit(1)

willow_config = {}

with open('build/config/sdkconfig.json') as sdkconfig:
    data = json.load(sdkconfig)

    # Iterating through the json
    for key in data:
        if key.startswith('WILLOW_'):
            value = data[key]
            willow_config.update({key: value})

# NVS supports a max key length of 15 characters, this will need a lot of work
with open('config/willow.csv', 'w') as csv:
    # Write column header
    csv.write("key,type,encoding,value\n")
    # Write namespace
    csv.write(f"{namespace},namespace,,\n")
    for key in willow_config.keys():
        encoding = type_condition(willow_config[key])
        nvs_key = key.replace("WILLOW_", "")
        csv.write(f"{nvs_key},namespace,{encoding},{willow_config[key]}\n")

# Get raw values
json_object = json.dumps(willow_config, indent=1)
 
# Write WILLOW_ values as-is to willow.json
with open("config/willow.json", "w") as outfile:
    outfile.write(json_object)