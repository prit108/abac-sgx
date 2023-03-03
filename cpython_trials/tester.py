from ctypes import *
shared_obj_file = "./module.so"

func = CDLL(shared_obj_file)

print(func.mult3(10))