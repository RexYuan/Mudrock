<div align="center">

# Mudrock

<img src="https://user-images.githubusercontent.com/5001650/198895789-3f067a37-8863-4455-851f-8e88f57f772c.png" height="500px" width="500px">

</div>

## Dependency

1. [gcc@10](https://formulae.brew.sh/formula/gcc@10)
2. [python@3.10](https://formulae.brew.sh/formula/python@3.10)

## Compilation

```zsh
make all      # same as make debug
make debug    # build without optimization and with debug info
make release  # build with optimization and without debug info
make clean    # delete builds
make cleanall # delete builds including libraries
```

## Usage

```zsh
./main.o d path/to/input.aag # run direct cdnf persistent learning
./main.o m path/to/input.aag # run mcmillan's interpolation + cdnf
```

## Testing

```zsh
./test.py # run d method, m method, and abc int against hwmcc10 benchmark
```

For configurations, see:

```
./test.py -h    
usage: test.py [-h] [-r RESULT] [-w WORKER] [--no-direct] [-f FILE [FILE ...] | -n NUMBER]

options:
  -h, --help            show this help message and exit
  -r RESULT, --result RESULT
                        filter sat/uns
  -w WORKER, --worker WORKER
                        specify number of workers
  --no-direct           do not test direct method
  -f FILE [FILE ...], --file FILE [FILE ...]
                        specify one or more input file title(s)
  -n NUMBER, --number NUMBER
                        specify number of tests
```

For example,

```zsh
./test.py -r sat -w 30 --no-direct # run m method and abc int against only the sat cases in hwmcc10 benchmark with 30 workers
```
