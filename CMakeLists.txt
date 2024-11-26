
cmake_minimum_required(VERSION 3.20)


project(Encrypx)


add_executable( encrypx

		src/config.h
		src/main.c

		src/encrypts_funcs.c
		src/input_funcs.c
		# Functions of Mixing
		src/mix_encrypt.c
		src/mix_encrypt.h


)

set_property(TARGET encrypx PROPERTY C_STANDARD 11)
