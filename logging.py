
import itertools

from config import *

# output title
ret_title        = "result"
state_title      = "#state"
aag_title        = "file"
donut_time_tile  = "donut time"
direct_time_tile = "direct time"
abc_time_title   = "abc int time"
diff_time_title  = "diff"

# ouput format
places_before_decimal = len(str(time_limit))
places_after_decimal  = 4

max_ret_wd   = max(itertools.starmap(lambda aag, state, ret : len(ret), tests))
max_state_wd = max(itertools.starmap(lambda aag, state, ret : len(state), tests))
max_aag_wd   = max(itertools.starmap(lambda aag, state, ret : len(aag), tests))
max_time_wd  = places_before_decimal+len(".")+places_after_decimal

ret_wd         = max(max_ret_wd, len(ret_title))
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
         f"{ret_title:<{ret_wd}}" + separator + \
         f"{state_title:<{state_wd}}" + separator + \
         f"{aag_title:^{aag_wd}}" + separator +\
         f"{donut_time_tile:^{donut_time_wd}}" + separator + \
         f"{abc_time_title:^{abc_time_wd}} ({diff_time_title:^{diff_time_wd}})" + separator + \
         f"{direct_time_tile:^{direct_time_wd}} ({diff_time_title:^{diff_time_wd}})" + \
         rightmost
def fmt_line(ret, state, aag, donut_time, abc_time, direct_time):
    def fmt_time(time):
        if type(time) is str:
            tmp = time
        else:
            tmp = f"{round(time, places_after_decimal):.{places_after_decimal}f}"
        return tmp
    def fmt_diff_time(time1, time2):
        if type(time1) is str or type(time2) is str:
            tmp = "-" * diff_time_wd
        else:
            tmp = f"{round(time1 - time2, places_after_decimal):+.{places_after_decimal}f}"
        return tmp
    tmp = leftmost + \
          f"{ret:^{ret_wd}}" + separator + \
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
def print_line(ret, state, aag, donut_time, abc_time, direct_time):
    print(fmt_line(ret, state, aag, donut_time, abc_time, direct_time), end="\n", flush=True)
