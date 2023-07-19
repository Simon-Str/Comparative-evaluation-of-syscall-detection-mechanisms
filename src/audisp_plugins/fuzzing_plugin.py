#!/usr/bin/env python3
import os
import sys
import signal 

interrupt_signal = True
logs_accumulated = list()
log_path = "/tmp/fuzzing_tmp_dir"
log_file = "seccomp_messages"

def signal_handler(sig, frame): 
    global interrupt_signal 
    interrupt_signal = False 
    write_to_file() 

def write_to_file(): 
    global log_path 
    global logs_accumulated
    with open(os.path.join(log_path, log_file), 'w') as opened_log_file:
        for line in logs_accumulated:
            opened_log_file.write(line)


def main(): 
    global interrupt_signal
    global logs_accumulated
    global log_path


    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    signal.signal(signal.SIGHUP, signal_handler)

    if not os.path.exists(log_path): 
        os.mkdir(log_path)

    while interrupt_signal: 
        for log_message in sys.stdin: 
            if "type=SECCOMP" in log_message: 
                logs_accumulated.append(log_message)

    


if __name__ == "__main__": 
    main()