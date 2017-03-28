# Operating-Systems

gerritse.adventure.c:
    
    A little adventure game where the goal is to get to the final room
  
    gcc -o gerritse.adventure.c Adventure
  
smallsh.c:
    
    C shell example. Using basic linux commands

    To compile: gcc -o smallsh.c Smallsh
  
  
  
  
TCP Encryption:

    An example of a one time pad encryption. 
    All execution, compiling, and testing of this program should ONLY be done in the bash prompt
    
    To Compile: Run the makefile.


    1. otp_enc_d: This program will run in the background. Its function is to perform the actual encoding. This program will listen on a particular port, assigned when it is first ran, and receives plaintext and a key via that port when a connection to it is made. It will then write back the ciphertext to the process that it is connected to via the same port.

    When otp_enc_d makes a connection with otp_enc, it forks off a separate process, and is available to receive more connections. It can support up to five concurrent socket connections. In the forked off process, the actual encryption will take place, and the ciphertext will be written back.

    otp_enc_d listening_port
    listening_port is the port that otp_enc_d should listen on. You will always start otp_enc_d in the background.

    $ otp_enc_d 57171 &
    
    2. otp_enc: This program connects to otp_enc_d, and asks it to perform a one-time pad style encryption as detailed above.Its syntax is as follows:

    otp_enc plaintext key port
    
    In this syntax, plaintext is the name of a file in the current directory that contains the plaintext you wish to encrypt. Similarly, key contains the encryption key you wish to use to encrypt the text. Finally, port is the port that otp_enc should attempt to connect to otp_enc_d on.

    When otp_enc receives the ciphertext, it should output it to stdout. Thus, otp_enc can be launched in any of the following methods, and should send its output appropriately:

    $ otp_enc myplaintext mykey 57171 $ otp_enc myplaintext mykey 57171 > myciphertext $ otp_enc myplaintext mykey 57171 > myciphertext &
    3. otp_dec_d: This program performs exactly like otp_enc_d, in syntax and usage. In this case, however, otp_dec_d will decrypt ciphertext it is given, using the passed-in ciphertext and key. Thus, it returns plaintext again to otp_dec.

    4. otp_dec: Similarly, this program will connect to otp_dec_d and will ask it to decrypt ciphertext using a passed-in ciphertext and key. It will use the same syntax and usage as otp_enc,

    5. keygen: This program creates a key file of specified length. The characters in the file generated will be any of the 27 allowed characters, generated using the standard UNIX randomization methods.
