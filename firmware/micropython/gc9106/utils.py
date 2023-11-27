'''!
@file
@author Vinícius Gabriel Linden
@date 2022-11-09
@copyright nubix Software-Design GmbH
@brief Utilities for debugging and stuff
'''

def hex_list(values : list[int]) -> str:
    '''
    Helper function
    '''
    return '[' + ', '.join([f'0x{i:02X}' for i in values]) + ']'

def pretty_dict(d : dict) -> str:
    '''
    Print dictionary in a more readable way
    '''
    r = "{\n"
    for key, value in d.items():
        if type(value) is dict:
            r += f"    {key}: "
            value = pretty_dict(value)
            value = value.replace("\n", "\n    ")
            r += value + ",\n"
        elif type(value) is list:
            r += f"    {key}: "
            if all(isinstance(x, int) for x in value):
                value = hex_list(value)
            else:
                value = str(value)
            value = value.replace(", ", ",\n        ")
            r += value + ",\n"
        else:
            r += f"    {key}: {value},\n"
    r = r[0:-2] + "\n}"
    return r
