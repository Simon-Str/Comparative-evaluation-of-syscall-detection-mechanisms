#!/usr/bin/env python3

import sys
import os
import signal 

# Bindings for audit and auparse are provided with python3-audit

log_path = "/tmp/syscall_file_fuzzer"
event_counter = 0 
accumulated_logs = list() 
file_cnt = 0 
event_limit = 1000
cleanup = True

def write_logs_to_file(): 
    global accumulated_logs 
    global log_path 
    global file_cnt 
    print("writing to file")
    file = open(log_path + "/" + str(file_cnt), 'w')

    for item in accumulated_logs :
        file.write(item)

    file.close() 
    file_cnt += 1 


            
def signal_handler(sig, frame): 
    global cleanup 
    global accumulated_logs
    if accumulated_logs: 
        write_logs_to_file()
    cleanup = False


def main(): 
    global file_cnt
    global log_path
    global current_event_global_scope
    global cleanup 
    global event_counter
    global event_limit

    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    signal.signal(signal.SIGHUP, signal_handler)
    
    if not os.path.exists(log_path): 
        os.mkdir(log_path)

    while cleanup: 
        for message in sys.stdin: 
            if ("type=SYSCALL" in message) or ("type=PATH" in message):
                accumulated_logs.append(message)
                event_counter += 1 
            
            if event_counter >= event_limit: 
                write_logs_to_file() 
                event_counter = 0
                accumulated_logs.clear() 
                

if __name__ == "__main__": 
    main()