
import pathlib

# test configs
time_limit = 900
term_time_limit = 30

# path configs
mudk_prefix = str(pathlib.Path(__file__).parent.absolute())
home_prefix = str(pathlib.Path.home())

hwmcc_prefix = home_prefix+"/Sources/hwmcc/hwmcc10-except-intel-aiger1"
assert pathlib.Path(hwmcc_prefix).exists()

abc_prefix = home_prefix+"/Sources/abc"
abc_exists = pathlib.Path(abc_prefix).exists()

prog = mudk_prefix+"/main.o"
abc = abc_prefix+"/abc"

log_prefix = mudk_prefix+"/log"
log_exists = pathlib.Path(log_prefix).exists()

test_cases = hwmcc_prefix+"/0test.txt"
# prepare test cases
tests = []
with open(test_cases) as f:
    for line in list(f):
        aag, state, ret = line.split()
        tests.append((aag, state, ret))

# logging
def log_out(aag, data):
    with open(f"{log_prefix}/{aag}.log", "w") as f:
        f.write(data)

class TermError(Exception):
    pass
