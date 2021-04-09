#! /usr/local/bin/python3

import timeit
import subprocess
import multiprocessing
import functools
import itertools
import pathlib

# path configs
mudk_prefix = str(pathlib.Path(__file__).parent.absolute())
home_prefix = str(pathlib.Path.home())

hwmcc_prefix = home_prefix+"/Sources/hwmcc/hwmcc10-except-intel-aiger1"
assert pathlib.Path(hwmcc_prefix).exists()

abc_prefix = home_prefix+"/Sources/abc"
assert pathlib.Path(abc_prefix).exists()

prog = mudk_prefix+"/main.o"
abc = abc_prefix+"/abc"

log_prefix = mudk_prefix+"/log"
if not pathlib.Path(log_prefix).exists():
    pathlib.Path(log_prefix).mkdir()

# prepare test cases
test_cases = hwmcc_prefix+"/0test.txt"
tests = []
with open(test_cases) as f:
    for line in list(f):
        aag, state, ret = line.split()
        tests.append((aag, state, ret))
tests = tests[:]

# subprocess options
time_limit = 900
run_opts = {
    "capture_output": True,
    "text":           True,
    "encoding":       "utf-8",
    "timeout":        time_limit,
}
run = functools.partial(subprocess.run, **run_opts)

# logging
def log_out(aag, data):
    with open(f"{log_prefix}/{aag}.log", "w") as f:
        f.write(data)

# run mudk
def run_in_mode(mode, aag):
    command = [prog, mode, f"{hwmcc_prefix}/aag/{aag}.aag"]
    ret = run(command)
    if mode == "m":
        log_out(aag, ret.stdout)
    return ret

# run abc
def run_abc_int(aig):
    c = f"read_aiger {hwmcc_prefix}/aig/{aig}.aig; int;"
    command = [abc, "-c", c]
    ret = run(command)
    return ret

# run `func` for `number` times and return average time
def avg_time(func, number):
    time = timeit.timeit(func, number=number)
    return time/number

# output title
state_title      = "#state"
aag_title        = "file"
donut_time_tile  = "donut time"
direct_time_tile = "direct time"
abc_time_title   = "abc int time"
diff_time_title  = "diff"

# ouput format
places_before_decimal = len(str(time_limit))
places_after_decimal  = 4

max_state_wd = max(itertools.starmap(lambda aag, state, ret : len(state), tests))
max_aag_wd   = max(itertools.starmap(lambda aag, state, ret : len(aag), tests))
max_time_wd  = places_before_decimal+len(".")+places_after_decimal

state_wd       = max(max_state_wd, len(state_title))+len("#")
aag_wd         = max(max_aag_wd, len(aag_title))
donut_time_wd  = max(max_time_wd, len(donut_time_tile))
direct_time_wd = max(max_time_wd, len(direct_time_tile))
abc_time_wd    = max(max_time_wd, len(abc_time_title))
diff_time_wd   = max(max_time_wd, len(diff_time_title))+len("+")

leftmost  = "| "
rightmost = " |"
separator = " | "

# output lines
header = leftmost + \
         f"{state_title:<{state_wd}}" + separator + \
         f"{aag_title:^{aag_wd}}" + separator +\
         f"{donut_time_tile:^{donut_time_wd}}" + separator + \
         f"{abc_time_title:^{abc_time_wd}} ({diff_time_title:^{diff_time_wd}})" + separator + \
         f"{direct_time_tile:^{direct_time_wd}} ({diff_time_title:^{diff_time_wd}})" + \
         rightmost
def fmt_line(state, aag, donut_time, abc_time, direct_time):
    def fmt_time(time):
        if time == "to":
            tmp = "to"
        else:
            tmp = f"{round(time, places_after_decimal):.{places_after_decimal}f}"
        return tmp
    def fmt_diff_time(time1, time2):
        if time1 == "to" or time2 == "to":
            tmp = "-" * diff_time_wd
        else:
            tmp = f"{round(time1 - time2, places_after_decimal):+.{places_after_decimal}f}"
        return tmp
    tmp = leftmost + \
          f"{'#'+state:<{state_wd}}" + separator + \
          f"{aag:>{aag_wd}}" + separator +\
          f"{fmt_time(donut_time):^{donut_time_wd}}" + separator + \
          f"{fmt_time(abc_time):^{abc_time_wd}} ({fmt_diff_time(abc_time, donut_time):^{diff_time_wd}})" + separator + \
          f"{fmt_time(direct_time):^{direct_time_wd}} ({fmt_diff_time(direct_time, donut_time):^{diff_time_wd}})" + \
          rightmost
    return tmp

# print lines
def print_horizontal():
    print("-" * len(header), end="\n")
def print_header():
    print(header, end="\n")
def print_line(state, aag, donut_time, abc_time, direct_time):
    print(fmt_line(state, aag, donut_time, abc_time, direct_time), end="\n", flush=True)

#####

runs = 2
def run_test(aag, state, ret):
    direct_time = (donut_time := "--")

    try:
        donut_time = avg_time(lambda: run_in_mode("m", aag), number=runs)
    except subprocess.TimeoutExpired:
        donut_time = "to"

    try:
        abc_time = avg_time(lambda: run_abc_int(aag), number=runs)
    except subprocess.TimeoutExpired:
        abc_time = "to"

    try:
        direct_time = avg_time(lambda: run_in_mode("d", aag), number=runs)
    except subprocess.TimeoutExpired:
        direct_time = "to"

    print_line(state, aag, donut_time, abc_time, direct_time)

worker = 2
if __name__ == "__main__":
    print_horizontal()
    print_header()
    print_horizontal()
    with multiprocessing.Pool(processes=worker) as pool:
        pool.starmap(run_test, tests)
    print_horizontal()
