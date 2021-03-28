#! /usr/local/bin/python3

import time
import subprocess

aags = [
    'linear.aag',
    # state size
    # 3
    'bj08aut1.aag',
    'bj08aut5.aag',
    'bj08aut62.aag',
    'bj08aut82.aag',
    # 5
    'bj08autg3f1.aag',
    'bj08autg3f2.aag',
    'bj08autg3f3.aag',
    'pdtvisgray0.aag',
    'pdtvisgray1.aag',
    # 10
    'pdtvispeterson.aag',
    'nusmvsyncarb5p2.aag',
    # 13
    'bobtutt.aag',
    'bobcount.aag',
    # 14
    'shortp0.aag',
    'shortp0neg.aag',
    # 15
    'visemodel.aag',
    # 16
    'counterp0.aag',
    'eijks208o.aag',
    'counterp0neg.aag',
    'pdtvisgigamax0.aag',
    'pdtvisgigamax1.aag',
    'pdtvisgigamax2.aag',
    'pdtvisgigamax3.aag',
    'pdtvisgigamax4.aag',
    'pdtvisgigamax5.aag',
    # 20
    'mutexp0.aag',
    'mutexp0neg.aag',
    'bobuns2p10d20l.aag',
    'nusmvsyncarb10p2.aag',
    # 21
    'neclaftp5001.aag',
    'neclaftp5002.aag',
    # 22
    'eijks208.aag',
    'viseisenberg.aag',
    'bjrb07amba1andenv.aag',
    # 23
    'eijks208c.aag',
    'visarbiter.aag',
    'vis4arbitp1.aag',
    # 24
    'pdtpmsudc8.aag',
    # 25
    'ringp0.aag',
    'ringp0neg.aag',
    'visbakery.aag',
]

encoding = 'utf-8'
timeout = 600
prog = "./build/main.o"
mode = "d"
prefix = "aag/"

aag_width = max(map(len, aags))+1

dsuc, msuc = False, False

for aag in aags:
    command = [prog, mode, prefix+aag]
    print(f'{aag:{aag_width}}', end=' ', flush=True)
    try:
        dstart = time.time()
        ret = subprocess.run([prog, "d", prefix+aag], capture_output=True, timeout=timeout)
        #output = str(ret.stdout, encoding=encoding)
        #print(output, end=' ')
        dend = time.time()
        dtime = dend-dstart
        dsuc = True
    except subprocess.TimeoutExpired:
        #print(f'>>> d spent over {timeout} seconds on {aag}')
        dsuc = False
    try:
        mstart = time.time()
        ret = subprocess.run([prog, "m", prefix+aag], capture_output=True, timeout=timeout)
        mend = time.time()
        mtime = mend-mstart
        msuc = True
    except subprocess.TimeoutExpired:
        #print(f'>>> m spent over {timeout} seconds on {aag}')
        msuc = False

    if dsuc and msuc:
        print(f'm-d = {mtime -dtime:3.3}')
    elif dsuc:
        print(f'd = {dtime:3.3}; m timed out')
    elif msuc:
        print(f'm = {dtime:3.3}; d timed out')
    else:
        print(f'both timed out')
