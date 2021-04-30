#! /usr/bin/env python3

import time
import subprocess
import multiprocessing
import functools
import signal

from config import *
from logging import *

if not log_exists:
    pathlib.Path(log_prefix).mkdir()

# subprocess options
run_opts = {
    "stdout":   subprocess.PIPE,
    "stderr":   subprocess.PIPE,
    "text":     True
}
run   = functools.partial(subprocess.run, **run_opts)
popen = functools.partial(subprocess.Popen, **run_opts)

# run mudk
def run_in_donut(aag):
    with popen([prog, "m", f"{hwmcc_prefix}/aag/{aag}.aag"]) as proc:
        try:
            output, _ = proc.communicate(timeout=time_limit)
        except subprocess.TimeoutExpired as e: # time out
            proc.send_signal(signal.SIGTERM)
            try:
                output, _ = proc.communicate(timeout=term_time_limit)
            except subprocess.TimeoutExpired as e: # cant gracefully terminate
                proc.kill()
                output, _ = proc.communicate()
                raise TermError from e
            else:
                raise e
        finally:
            log_out(aag, output)

def run_in_direct(aag):
    return run([prog, "d", f"{hwmcc_prefix}/aag/{aag}.aag"], timeout=time_limit)

# run abc
def run_abc_int(aig):
    c = f"read_aiger {hwmcc_prefix}/aig/{aig}.aig; int;"
    command = [abc, "-c", c]
    ret = run(command, timeout=time_limit)
    return ret

def get_time(func):
    start_time = time.time()
    func()
    end_time = time.time()
    return end_time-start_time

# run test
def run_test(aag, state, ret):
    direct_time = (donut_time := "--")

    try:
        donut_time = get_time(lambda: run_in_donut(aag))
    except subprocess.TimeoutExpired:
        donut_time = "to"
    except TermError:
        donut_time = "killed"

    if abc_exists:
        try:
            abc_time = get_time(lambda: run_abc_int(aag))
        except subprocess.TimeoutExpired:
            abc_time = "to"
    else:
        abc_time = "n/a"

    try:
        direct_time = get_time(lambda: run_in_direct(aag))
    except subprocess.TimeoutExpired:
        direct_time = "to"

    print_line(state, aag, donut_time, abc_time, direct_time)

if __name__ == "__main__":
    print_horizontal()
    print_header()
    print_horizontal()
    with multiprocessing.Pool(processes=worker) as pool:
        pool.starmap(run_test, tests)
    print_horizontal()
