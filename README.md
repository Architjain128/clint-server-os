# How to run
after going in folder 2019101053_assign6
* in one terminal run this
    ``` 
    -> cd server
    -> gcc server.c -o server
    -> ./server
    ```
+ in other terminal run this
    ```
    -> cd client
    -> gcc client.c -o client
    -> ./client
    ```
+ when `exit` is given as input in client both client and server otherwise signal handeling is not considered (if exited by signals in one program other will not respond to it and keeps running)
+ after exiting by signals we need to kill the running (if  shown in list of `ps`)
+ `random.py` was used for generating random file for testing or by `base64 /dev/urandom | head -c size > file.txt`
+ transfered file was checked by running command `diff ./server/file_name ./client/file_name` in parent directory of sever and client
# Directory structure
      ./2019101053_assign6
        |
        ├── README.md
        ├── random.py
        ├── client
        │   ├── client.c
        |   └──(other files)
        └── server
            ├── server.c
            └──(other files)
    
