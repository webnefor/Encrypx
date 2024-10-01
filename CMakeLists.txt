
cmake_minimum_required(VERSION 3.20)


project(nefor)



add_executable( nefor

	src/config.h
        src/nefor.c
        src/f_encrypt.c
        src/strf.c

)

set_property(TARGET nefor PROPERTY C_STANDARD 11)

