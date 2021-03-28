#! /usr/local/bin/python3

import timeit
import subprocess
import functools
import itertools

# prepare test cases
tests = []
with open('aag/0test.txt') as f:
    for line in list(f):
        aag, state, ret = line.split()
        tests.append((aag, state, ret))

# program options
prog    = "./build/main.o"
prefix  = "aag/"
postfix = ".aag"

# subprocess options
run_opts = {
    'capture_output': True,
    'text':           True,
    'encoding':       'utf-8',
    'timeout':        600,
}

# subprocess dispatch
run = functools.partial(subprocess.run, **run_opts)
def run_in_mode(mode, aag):
    command = [prog, mode, prefix+aag+postfix]
    return run(command)
def avg_time(func, number):
    time = timeit.timeit(func, number=number)
    return time/number

# format parameters
state_title     = '#state'
filename_title  = 'filename'
d_time_title    = 'd time'
m_time_title    = 'm time'
diff_time_title = 'diff'

s_width   = max(itertools.starmap(lambda f,s,r : int(s), tests)) // 10
aag_width = max(itertools.starmap(lambda f,s,r : len(f), tests))

decimal_width  = 3
time_precision = 2

state_width    = max(len(state_title), s_width)
filename_width = max(len(filename_title), aag_width)
time_width     = max(len(d_time_title), decimal_width+time_precision+1)

# output format
header = f'{state_title:^{state_width}}  ' \
         f'{filename_title:^{filename_width}} | ' \
         f'{d_time_title:^{time_width}} | ' \
         f'{m_time_title:^{time_width}} ({diff_time_title:^{time_width+1}})'

def fmt_filename(aag):
    return f'{aag:>{filename_width}}'
def fmt_state(s):
    return f'{"#"+s:>{state_width}}'
def fmt_time(t):
    if type(t) is str:
        return f'{"to":^{time_width}}'
    return f'{t:{time_width}.{time_precision}f}'
def fmt_diff(t1, t2):
    if type(t1) is str or type(t2) is str:
        return f'{"-"*(time_width+1):^{time_width+1}}'
    return f'{t1-t2:+{time_width+1}.{time_precision}f}'

def print_header():
    print(header, end='\n')
def print_before_running(s, aag):
    tmp = f'{fmt_state(s)} ' \
          f'{fmt_filename(aag)}'
    print(tmp, end='\r', flush=True)
def print_after_running(s, aag, d_time, m_time):
    tmp = f'{fmt_state(s)}  ' \
          f'{fmt_filename(aag)} | ' \
          f'{fmt_time(d_time)} | '\
          f'{fmt_time(m_time)} ({fmt_diff(m_time, d_time)})'
    print(tmp, end='\n')

#####

runs = 1
print_header()
for aag,s,_ in tests:
    print_before_running(s, aag)
    d_time = (m_time := "--")

    try:
        d_time = avg_time(lambda: run_in_mode("d", aag), number=runs)
    except subprocess.TimeoutExpired:
        d_time = "to"

    try:
        m_time = avg_time(lambda: run_in_mode("m", aag), number=runs)
    except subprocess.TimeoutExpired:
        m_time = "to"

    print_after_running(s, aag, d_time, m_time)
