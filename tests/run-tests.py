# Small testing script
import json
import os
import subprocess

GREEN = '32'
RED = '31'

cur_file = os.path.abspath(__file__) 
src_dir = os.path.dirname(os.path.dirname(cur_file))

def compile_btree():

    compile_proc = subprocess.run(['make', 'clean', '&&', 'make'],
                                cwd=src_dir,
                                shell=True, 
                                check=True)
    if compile_proc.returncode != 0:
        print(f"Compilation failed")     

def print_colored(text, color):
    print(f"\033[{color}m{text}\033[0m")

def run_test(test_file):
    run_proc = subprocess.run(['./btree', '-t', os.path.join("tests", test_file)], 
                              cwd=src_dir,
                              stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE,
                              text=True)  

    print('-'*20) 
    print(test_file)
    if run_proc.returncode == 0:
        results = json.loads(str(run_proc.stdout))
        for test, result in results.items():
            print(f"{test}\t", end='')
            if result == 1:
                print_colored("Success", GREEN)
            else:
                print_colored("Failure", RED) 
        # for line in run_proc.stdout.split("\n"):
        #     line = line.strip()
        #     if (line.endswith('0')):
        #         print('X', end='')
        #     else:
        #         print('Y', end='')
        #     print(line + " ", end='')
        # print()
    else:
        print("tests crashed")
        print(run_proc.stderr)

if __name__ == "__main__":

    test_files = os.listdir(os.path.dirname(cur_file))

    compile_btree()
    
    for file in test_files:
        if (file.endswith(".txt")):
            run_test(file)