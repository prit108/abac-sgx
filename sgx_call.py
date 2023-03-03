from ctypes import *

def csgx_mult3(input: int):
    so_file = "./App/App.so"
    functions = CDLL(so_file)
    answer = functions.main(input)
    return answer

# if __name__ == "__main__":
#     i = int(input("Enter an integer: "))
#     print(csgx_mult3(i))
