import random 

import sys
if __name__ == "__main__":
    
    size = 10
    file_name = "in.txt"
    mode = "-s"

    if len(sys.argv) > 1: 
        file_name = sys.argv[1]
        size = int(sys.argv[2])
        mode = sys.argv[3]

     
    with open(file_name, "w") as file:
        if mode == "-r":
            numbers = random.sample(range(size), size) 
            for x in numbers:
                file.write(str(x) + "\n")
        elif mode == "-s":
            for i in range(size): 
                file.write(str(i) + "\n")

    # file.write("")
