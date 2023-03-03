from ctypes import *
shared_obj_file = "./App/App.so"
# enclave_obj_file = "./enclave.so"
func = CDLL(shared_obj_file)
# enclave = CDLL(enclave_obj_file)

# if(func.initialize_enclave() < 0):
#     print("Failed to initialize enclave.")
#     exit(-1)
# global_eid = func.get_global_enclave_id()
# print(global_eid)
# print(enclave.printf_helloword(global_eid))

# output = enclave.mult3(global_eid, 10)
# print(func.printf_helloword(global_eid))

# output = func.mult3_transformer(10)
answer = func.main(12)
print(answer)
