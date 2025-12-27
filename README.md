# Bash_in_c

## introduce

The simple shell with C

## feature command
- paul: print the testing text
- history:show history record with command
- other: same command from origin bash

## install package
```bash
sudo apt-get install -y libreadline-dev
```

## compile way

```bash
gcc -Wall -Wextra -o main main.c -lreadline
```

or

```bash
make
```


## reference
- https://github.com/danishprakash/dash/tree/master  
- https://danishpraka.sh/posts/write-a-shell/

