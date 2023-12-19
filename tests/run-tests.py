import pytest
import os
import subprocess

def compile_btree():
    compile_proc = subprocess.run(['make', 'clean', '&&', 'make'], shell=True, check=True) 

    if compile_proc.returncode != 0:
        print(f"Compilation failed")     

def run_test(test_file):
    run_proc = subprocess.run(['./btree', test_file], 
                              stderr=subprocess.PIPE,
                              text=True)  
    if run_proc.returncode == 0:
        print("success")
    else:
        print("failed")
        print(run_proc.stderr)

if __name__ == "__main__":

    test_files = os.listdir(os.path.dirname(__file__))

    for file in test_files:
        # run tests
        print(file)
