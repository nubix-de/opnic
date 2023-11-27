#!/usr/bin/env python3
'''Get Unity map coordinates.

Author Cassiano Silva de Campes
Date 2023-08-08
Copyright nubix Software-Design GmbH
'''

from csv import DictReader

# Define the C-structure template
struct_typedef = '''\
typedef struct Coordinate_t {
  int x;             ///< x-coordinate
  int y;             ///< y-coordinate
  bool visible;      ///< is the object visible in viewport
  bool picked;       ///< was the object picked up (if is pickable)
  uint8_t width;     ///< width of the object
  uint8_t height;    ///< height of the object
  int32_t decay;     ///< alpha decay (if have)
  const char *text;  ///< Text of the object (if have)
} Coordinate_t;
'''
struct_template = '''\
static Coordinate_t {name1}[static_cast<int>(WorldObjects::k{name2})] = {{
        {data}}};
'''
size_template = "  k{name} = {size},\n"

# Read the CSV file and store the coordinates for each name
coordinates_dict = {}
structure_sizes = {}

def get_dimmensions(name):
    try:
        return {
                "BlackHoles"            : [104, 104],
                "Bumpers"               : [18, 18],
                "Headlines"             : [0, 0],
                "NubixLogos"            : [138,65],
                "Boosters"              : [19, 19],
                "Collectables"          : [4,4],
                "Pickups"               : [64, 64],
                "Decorative_Hexagons"   : [45, 39],
                "6EArcades"             : [104,63],
                "FadingTextAreas"       : [10, 10],
                "CButtons"              : [51, 63],
                "DButtons"              : [51, 63],
                "Bodys"                 : [0, 0],
                #"BorderBumpers"         : [18, 18],
                }[name]
    except KeyError:
        return [0, 0]

min_x=float('inf')
min_y=float('inf')
max_x=float('-inf')
max_y=float('-inf')

with open('abc.csv', newline='') as csvfile:
    reader = DictReader(csvfile)
    for row in reader:
        name = row['name']
        y = 10000 - int((float(row['x']) * 13.27) + 1000)
        x = int((float(row['y']) * 13.27)  + 1000)

        min_x = min(min_x, x)
        min_y = min(min_y, y)
        max_x = max(max_x, x)
        max_y = max(max_y, y)

        sizeX, sizeY = get_dimmensions(name + "s")
        text = row['text']

        if not len(text):
            text = "NULL"

        coordinate = "    {"
        if text == "NULL":
            coordinate += f".x={x}, .y={y}, .visible=0, .picked=0, .width={sizeX}, .height={sizeY}, .decay=0, .text={text}"
        else:
            if name == "Headline":
                text_split = text.split("\\n")
                text = text_split[1]

            coordinate += f".x={x}, .y={y}, .visible=0, .picked=0, .width={sizeX}, .height={sizeY},.decay=0, .text=\"{text}\""

        coordinate += "}"

        if name == "6EArcade":
            name = "_6EArcade" # Add '_' to not start variable with number

        if name in coordinates_dict:
            coordinates_dict[name].append(coordinate)
        else:
            coordinates_dict[name] = [coordinate]

# Calculate the border bumpers quantity
#width = abs(min_x - max_x) + 100
#height = abs(min_y - max_y) + 100
#num_rows = height // 18
#num_cols = width // 18

# Calculate structure sizes
for name, coordinates in coordinates_dict.items():
    structure_sizes[name] = len(coordinates)

# Generate the C-structure code
c_struct_code = ""
c_size_code = ""

for name, coordinates in coordinates_dict.items():
    data = ",\n".join(coordinates)
    size = structure_sizes[name]
    plural = name + "s"
    c_struct_code += struct_template.format(name1=plural, name2=plural, data=data)
    c_struct_code += "\n"
    c_size_code += size_template.format(name=plural, size=size)

# Generate the Corner-bumpers
corner_coordinates=""

#for row in range(num_rows):
#  for col in range(num_cols):
#    x = col * 18
#    y = col * 18
#
#    sizeX, sizeY = get_dimmensions("BorderBumpers")
#    corner_coordinates += "    {"
#    corner_coordinates += f".x={x}, .y={y}, .visible=0, .picked=0, .width={sizeX}, .height={sizeY}, .decay=0, .text=NULL"
#    corner_coordinates +="},"
#    corner_coordinates += '\n'

#c_struct_code += struct_template.format(name1="BorderBumpers", name2="BorderBumpers", data=corner_coordinates)
#c_size_code += size_template.format(name="BorderBumpers", size=num_rows * num_cols)

output_filename = "Coordinates.hpp"

with open(output_filename, "w") as output_file:
    code = f'''\
#ifndef COORDINATES_H
#define COORDINATES_H

#include <stdint.h>

#define MIN_X ({min_x})
#define MIN_Y ({min_y})
#define MAX_X ({max_x})
#define MAX_Y ({max_y})

{struct_typedef}

enum class WorldObjects : int {{
{c_size_code}
}};

{c_struct_code}

#endif /* COORDINATES_H */
'''
    output_file.write(code)
    print(code)

#c_struct_code += struct_template.format(name1="BorderBumpers", name2="BorderBumpers", data=corner_coordinates)
#c_struct_code += "\n"
#c_size_code += size_template.format(name="BorderBumpers", size=num_rows * num_cols)
#print(c_size_code)
