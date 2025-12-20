# Bash_in_c

## introduce

The lightweight shell with C

## feature command
- paul: print the text
- history:show history record with command
- other shell command from origin command，but some doesn't work

## install package
```bash
sudo apt-get install -y libreadline-dev
```

## compile way

```bash
gcc -Wall -Wextra -o main main.c -lreadline
```

## to-do
implement other command


## reference
- https://github.com/danishprakash/dash/tree/master  
- https://danishpraka.sh/posts/write-a-shell/

