import os 
import sys

def init_custom_mutator(path_custom_mutator_dir: str, package_name: str) -> bool: 
    print("[+] Setting the custom mutator environment variables.") 
    tmp = package_name + ".py"
    
    if not os.path.exists(path_custom_mutator_dir) and not os.path.exists(os.path.join(path_custom_mutator_dir, tmp)): 
        print("[!] The path to the custom mutator does not exist")
        return False 

    os.environ['PYTHONPATH'] = path_custom_mutator_dir
    os.environ['AFL_PYTHON_MODULE'] = package_name

    return True

def custom_mutator_only()-> None: 
    print("[+] Setting environment variable for custom mutator usage only.")
    os.environ['AFL_CUSTOM_MUTATOR_ONLY'] = "1"

def unset_custom_mutator_variables():
    print("[+] Unsetting environment variables for custom mutators")
    
    if 'PYTHONPATH' in os.environ and "AFL_PYTHON_MODULE" in os.environ:
        os.environ.pop('PYTHONPATH')
        os.environ.pop('AFL_PYTHON_MODULE')

def unset_custom_mutator_only():   
    os.environ.pop('AFL_CUSTOM_MUTATOR_ONLY')
    
if __name__ == "__main__": 
    # python3 /fuzzing_resources/afl_custom_mutators/weighted_input_mutator/prep_environment.py /fuzzing_resources/afl_custom_mutators/weighted_input_mutator weighted_input_mutator
    init_custom_mutator(sys.argv[1], sys.argv[2])
    custom_mutator_only() 
    unset_custom_mutator_variables()
    unset_custom_mutator_only()