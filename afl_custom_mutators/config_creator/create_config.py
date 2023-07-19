import configparser
import sys
import os 

def create_config(save_path: str, locations_path:str, existing_flags_paths:str)-> bool: 
    filename = "custom_mutator.ini" 

    if not os.path.exists(locations_path) and not os.path.exists(existing_flags_paths) and not os.path.exists(save_path): 
        print("[!] One of the given paths does not exist.")
        return False 


    config = configparser.ConfigParser()
    config['weighted_input_mutator'] = {'locations_path': locations_path,
                                        'existing_flags_path': existing_flags_paths}


    with open(os.path.join(save_path,filename), 'w') as configfile: 
        config.write(configfile)

if __name__ == "__main__": 
    #python3 ./create_config /fuzzing_resources/afl_custom_mutators/weighted_input_mutator /fuzzing_resources/afl_custom_mutators/arg_and_file_input/rootlocation.txt /fuzzing_resources/afl_custom_mutators/weighted_input_mutator/exprargs_transformed_input.txt
    if not create_config(sys.argv[1], sys.argv[2], sys.argv[3]): 
        sys.exit(-1)