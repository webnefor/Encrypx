Encrypx:

    -f a file for encrypting [-f example.zip]
 
    -t a trash file [-t trash] (in developing)
  
    -k password of encrypting [-k 12345]
  
    -m has two params, like: [encrypt & decrypt]

build:

    cmake . && cmake --build .


example:

    ./encrypx -f yourfile.zip -t yourfile.zip -m encrypt -k qwerty123
