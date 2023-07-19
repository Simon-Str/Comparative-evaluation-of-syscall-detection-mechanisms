#!/usr/bin/env python3
# encoding: utf-8

import random
import string
import os 
import configparser


include_files = list() 
valid_flags = list 

set_of_valid_characters = ""
locations_path = ""
existing_flags_path = ""
havoc_mutation_method = "h"

consider_stdin = False 
havoc_mutation_consider_stdin = False 
post_process_flag = False 


def init(seed):
    global include_files
    global valid_flags
    global locations_path 
    global existing_flags_path
    global set_of_valid_characters
    global consider_stdin 
    global havoc_mutation_consider_stdin
    global havoc_mutation_method

    random.seed(seed)

    config = configparser.ConfigParser()
    file_path = os.path.abspath(__file__) 
    file_path = os.path.dirname(file_path)
    config_file = os.path.join(file_path, "custom_mutator.ini")
    config.read(config_file)
    print(config.sections())
    locations_path = config['weighted_input_mutator']['locations_path']
    existing_flags_path = config['weighted_input_mutator']['existing_flags_path']
    consider_stdin = config.getboolean('weighted_input_mutator', 'consider_stdin') 
    valid_character_flag = config['weighted_input_mutator']['valid_characters']

    havoc_mutation_consider_stdin = config.getboolean('custom_havoc_mutation','stdin_aware')
    havoc_mutation_method = config['custom_havoc_mutation']['havoc_mutation_method']

    if valid_character_flag == 'a': 
        set_of_valid_characters = string.ascii_letters + string.digits
    elif valid_character_flag == 'l': 
        set_of_valid_characters = string.ascii_letters
    elif valid_character_flag == 'd':
        set_of_valid_characters = string.digits

    with open(locations_path, "r") as location_file: 
        root_location = location_file.read().strip()

    with open(existing_flags_path, "r") as existing_flags_file: 
        valid_flags = existing_flags_file.readlines() 

    for (dirpath, dirnames, filenames) in os.walk(root_location): 
        for file in filenames: 
            include_files.append(os.path.join(dirpath, file))

    tmp_string = ""
    
    '''for iterator in range(5): 
        tmp_string = "/"
        for i in range(5): 
            for x in range(10): 
                tmp_string += random.choice(string.ascii_letters)
            tmp_string+= "/"
        
        include_files.append(tmp_string)'''


def deinit():
    pass


def fuzz(buf, add_buf, max_size):
    ''' This functions determine which fuzzing strategy will be used and which weight is assigned to it. 

    It will not create any input and instead just calls the according function that is going to build the input argument vector. 
    '''
    global set_of_valid_characters
    global post_process_flag
    post_process_flag = False

    #print(buf.decode())
    
    ret_value = choose_input_method()

    while len(ret_value) > 1000: 
        ret_value = choose_input_method() 

    ret_value += bytearray(((1000 - len(ret_value) - 2) * ' ').encode())
    if consider_stdin: 
        number_of_letters = random.randint(1, 10) 
        rand_input_string = ""
        for i in range(number_of_letters): 
                rand_input_string += random.choice(set_of_valid_characters) 

        ret_value += bytearray("\0\0".encode())
        rand_input_string = bytearray(rand_input_string.encode())
        ret_value += rand_input_string


    return ret_value




# def init_trim(buf):
#     return steps
#
# def trim():
#     return bytearray(...)
#
# def post_trim(success):
#
#     return next_index
#

def post_process(buf):
    global havoc_mutation_consider_stdin
    global post_process_flag
    global valid_flags
    
    if post_process_flag:
        flag = random.choice(valid_flags).strip() 
        bytearray_flag = bytearray(flag.encode())
        flag = random.choice(valid_flags).strip() 
        bytearray_flag += bytearray(flag.encode())
        
        bytearray_flag += bytearray(((1000 - len(bytearray_flag) ) * ' ').encode())
    
        if len(buf) > 1000: 
            buf = bytearray_flag + buf[1000:]
        else: 
            buf = bytearray_flag + buf

        return buf
    else:
        return buf
    




def havoc_mutation(buf, max_size):
    global post_process_flag

    post_process_flag = True 
    return buf 

'''def havoc_mutation(buf, max_size):
    global havoc_mutation_consider_stdin
    global havoc_mutation_method
    global valid_flags

    random.choice(valid_flags)

    if havoc_mutation_consider_stdin: 
        bytearray_flag = bytearray(random.choice(valid_flags).encode())
        bytearray_flag += bytearray(((1000 - len(bytearray_flag) - 2) * ' ').encode())
        if havoc_mutation_method == "h": 
            buf = rand_havoc(buf, max_size - len(bytearray_flag))
        elif havoc_mutation_method == "a": 
            buf = ascii_havoc(buf, max_size - len(bytearray_flag)) 

        buf = bytearray_flag + buf

        #print(f"HAVOC CONSIDER STDIN:::{buf}")

    else: 
        if havoc_mutation_method == "h": 
            return rand_havoc(buf, max_size)
        elif havoc_mutation_method == "a": 
            return ascii_havoc(buf, max_size)

def rand_havoc(buf, max_size): 
    buf = bytearray(buf)
    cust_ind = random.randint(0,len(buf)-1)
    havoc_byte = random.randint(0,127)

    buf[cust_ind] = buf[cust_ind] ^ havoc_byte

    if len(buf) > max_size: 
        buf = buf[len(buf)-max_size:]


    return buf
 

def ascii_havoc(buf, max_size): 
    buf = bytearray(buf)
    loop_iterations = random.randint(0,1000)
    
    for i in range(loop_iterations):
        havoc_byte = random.randint(33,126)

        if (i%3) == 0: 
            cust_ind = random.randint(0,len(buf)-1)
            buf[cust_ind] = havoc_byte
        else: 
            buf.extend(bytes(chr(havoc_byte).encode()))

    buf.replace
    if len(buf) > max_size: 
        buf = buf[len(buf)-max_size:]

    return buf'''

# def havoc_mutation_probability():
#     return prob
#
# def queue_get(filename):
#     return True
#
# def queue_new_entry(filename_new_queue, filename_orig_queue):
#     pass 


def choose_input_method() -> bytearray: 
    choice = random.randint(0,9)

    if choice < 1: 
        ret_value = rand_input()
    elif choice >= 1 and choice < 2: 
        ret_value = rand_input_with_file()
    elif choice >= 2 and choice < 6: 
        ret_value = existing_flag()
    elif choice >= 6 and choice < 8: 
        ret_value = existing_flag_with_file()
    elif choice >= 8: 
        ret_value = existing_flag_with_multiple_files()

    """if choice == 0: 
        ret_value = rand_input()
    elif choice == 1: 
        ret_value = rand_input_with_file()
    elif choice == 2: 
        ret_value = existing_flag()
    elif choice >= 3 and choice < 9: 
        ret_value = existing_flag_with_file()
    elif choice == 9: 
        ret_value = existing_flag_with_multiple_files()"""

    return ret_value 


def rand_input()-> bytearray: 
    ''' This methods provides inputs in the form of 1-3 invalid input flags. 
    ''' 
    result_string = "" 
    number_of_flags = random.randint(1, 3) 

    for i in range(number_of_flags): 
        number_of_chars= random.randint(1,4) 
        result_flag = "-"
        for i in range(number_of_chars): 
            result_flag += random.choice(string.ascii_letters) 
        result_flag += " " 
        result_string += result_flag
    
    return bytearray(result_string.encode())


def rand_input_with_file()-> bytearray: 
    ''' This methods provides inputs in the form of 1-3 invalid input flags and a file path. 
    ''' 
    result_string = "" 
    number_of_flags = random.randint(1, 3) 

    for i in range(number_of_flags): 
        number_of_chars= random.randint(1,4) 
        result_flag = "-"
        for i in range(number_of_chars): 
            result_flag += random.choice(string.ascii_letters) 
        result_flag += " " 
        result_string += result_flag

    result_string +=  random.choice(include_files).strip()    
 
    return bytearray(result_string.encode())


def existing_flag()-> bytearray: 
    ''' This methods provides inputs in the form of 1-3 valid input flags but not an existing file. 
    ''' 
    global valid_flags

    result_string = "" 
    number_of_flags = random.randint(3, 4) 
    #number_of_flags = random.randint(2, 3) 

    for i in range(number_of_flags): 
        result_string = random.choice(valid_flags)
        result_string += " "
    
    return bytearray(result_string.encode())


def existing_flag_with_file()-> bytearray: 
    ''' This methods provides inputs in the form of 1-3 valid input flags and a file path. 
    '''
    global valid_flags
    global include_files

    result_string = "" 
    #number_of_flags = random.randint(1, 3) 
    number_of_flags = random.randint(3, 4) 
    #number_of_flags = random.randint(5, 7) 
    #number_of_flags = 2

    for i in range(number_of_flags): 
        result_string += random.choice(valid_flags).strip()
        result_string += " "
    
    
    result_string +=  random.choice(include_files)
    return bytearray(result_string.encode()) 


def existing_flag_with_multiple_files()-> bytearray: 
    ''' This methods provides inputs in the form of 1-3 valid input flags and a file path. 
    '''
    global valid_flags
    global include_files

    result_string = "" 
    #number_of_flags = random.randint(1, 3) 
    number_of_flags = random.randint(3, 4) 

    for i in range(number_of_flags): 
        result_string += random.choice(valid_flags).strip()
        result_string += " "
        for x in range(2): 
            if random.randint(0,1) == 1: 
                result_string +=  random.choice(include_files).strip()
                result_string += " "
    
    #print(result_string)
    return bytearray(result_string.encode()) 


