#! /usr/bin/env python3

import time
import subprocess
import multiprocessing
import functools
import signal
import argparse

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

def validate_result(ret, ans):
    if ret == "Unknown":
        return True
    if ret == "Perfect" and ans == "uns":
        return True
    if ret == "Refuted" and ans == "sat":
        return True
    return False

# run mudk
def run_in_donut(aag, ret):
    command = [prog, "m", f"{hwmcc_prefix}/aag/{aag}.aag"]
    with popen(command) as proc:
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

            if not validate_result(output.split('\n')[-2], ret):
                raise RetError

def run_in_direct(aag, ret):
    command = [prog, "d", f"{hwmcc_prefix}/aag/{aag}.aag"]
    output = run(command, timeout=time_limit).stdout

    if not validate_result(output.split('\n')[-2], ret):
        raise RetError

# run abc
def run_abc_int(aig, ret):
    command = [abc, "-c", f"read_aiger {hwmcc_prefix}/aig/{aig}.aig; int;"]
    output = run(command, timeout=time_limit).stdout

def get_time(func):
    start_time = time.time()
    func()
    end_time = time.time()
    return end_time-start_time

# run test
def run_test(aag, state, ret, no_direct=False):
    result = [None, None, None]
    direct_time = (donut_time := "--")

    try:
        donut_time = get_time(lambda: run_in_donut(aag, ret))
    except subprocess.TimeoutExpired:
        donut_time = "to"
        result[0] = False
    except TermError:
        donut_time = "killed"
        result[0] = False
    except RetError:
        donut_time = "wrong"
        result[0] = False
    else:
        result[0] = True

    if abc_exists:
        try:
            abc_time = get_time(lambda: run_abc_int(aag, ret))
        except subprocess.TimeoutExpired:
            abc_time = "to"
            result[1] = False
        else:
            result[1] = True
    else:
        abc_time = "n/a"

    if no_direct:
        direct_time = "skipped"
        result[2] = False
    else:
        try:
            direct_time = get_time(lambda: run_in_direct(aag, ret))
        except subprocess.TimeoutExpired:
            direct_time = "to"
            result[2] = False
        except RetError:
            direct_time = "wrong"
            result[2] = False
        else:
            result[2] = True

    print_line(ret, state, aag, donut_time, abc_time, direct_time)
    return result

parser = argparse.ArgumentParser()
parser.add_argument("-r", "--result", help='filter sat/uns')
parser.add_argument("-w", "--worker", help='specify number of workers', default="1", type=int)
parser.add_argument("--no-direct", help='do not test direct method', action="store_true")
group = parser.add_mutually_exclusive_group()
group.add_argument("-f", "--file", help='specify one or more input file title(s)', nargs="+")
group.add_argument("-n", "--number", help='specify number of tests', type=int)

if __name__ == "__main__":
    args = parser.parse_args()
    if args.file:
        files = [f if "." not in f else f.split(".")[0] for f in args.file]
        tests = [(aag, state, ret) for aag, state, ret in tests if aag in files]
        if not tests:
            raise FileNotFoundError(args.file)
    if args.result:
        if args.result not in {"sat", "uns"}:
            raise ValueError(args.result)
        tests = [(aag, state, ret) for aag, state, ret in tests if ret == args.result]
    if args.number:
        tests = tests[:args.number]
        if not tests:
            raise IndexError(args.number)
    if args.no_direct:
        tests = [(aag, state, ret, True) for aag, state, ret in tests]

    print_horizontal()
    print_header()
    print_horizontal()
    with multiprocessing.Pool(processes=args.worker) as pool:
        rets = pool.starmap(run_test, tests)
    print_horizontal()

    print("total", len(tests))
    for n, i in zip(("donut", "abc", "direct"), range(3)):
        extracted = map(lambda trip: trip[i], rets)
        summed = functools.reduce(lambda acc,i: acc+1 if i else acc, extracted, 0)
        print(n, summed)
