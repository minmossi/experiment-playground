#!/usr/bin/env python3

# Set Arguments
import argparse
program_description = 'sockcntl - Check Existing Socket Characteristic'
parser = argparse.ArgumentParser(description=program_description)
# Mandatory
parser.add_argument('path', metavar='path', type=str, help='socket file path')

# Parse Arguments
args = parser.parse_args()
# Mandatory
path = args.path

# Connect Socket
import os, socket, fcntl, sys
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
sock.connect(path)

# Check O_ASYNC Support
init_flags = fcntl.fcntl(sock, fcntl.F_GETFL)
print('init_flags: ' + format(init_flags, '#064b'))
print('os.O_ASYNC: ' + format(os.O_ASYNC, '#064b'))
print('init_flags & os.O_ASYNC: ' + format(init_flags & os.O_ASYNC, '#064b'))
print('')
fcntl.fcntl(sock, fcntl.F_SETFL, init_flags | os.O_ASYNC) # O_ASYNC has to be set with fcntl()
flags_return = fcntl.fcntl(sock, fcntl.F_GETFL)
print('flags_return: ' + format(flags_return, '#064b'))
print('os.O_ASYNC: ' + format(os.O_ASYNC, '#064b'))
print('flags_return & os.O_ASYNC: ' + format(flags_return & os.O_ASYNC, '#064b'))

from termcolor import colored
if flags_return & os.O_ASYNC:
    print(colored('This file(socket) supports O_ASYNC flag!', 'green'))
else:
    print(colored('This file(socket) doesn\'t support O_ASYNC flag!', 'red'))
print('')

# Register SIGIO Signal Handler
import signal, time
def sigio_handler(sig, frame):
    end_time = time.perf_counter_ns()
    print(colored('SIGIO detected!', 'green'))
    print('latency = ' + str(end_time - start_time) + 'ns')
    sys.exit(0)
signal.signal(signal.SIGIO, sigio_handler)

# Set PID for SIGIO
fcntl.fcntl(sock, fcntl.F_SETOWN, os.getpid()) # registering PID of otherside process is mandatory for SIGIO to happen

# Open Another Socket
sock2 = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
sock2.connect(path)

# Write to Socket
start_time = time.perf_counter_ns()
sock2.send(b'0')

# Delay
print('Wait for maximum 5 seconds...')
time.sleep(5)
print(colored('SIGIO not detected!', 'red'))
sys.exit(1)