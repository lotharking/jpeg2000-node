# Compile
```c
gcc -DDEBUG convert.c -o convert -static -lopenjp2 -lm
```
>Use -DDEBUG flag only for dev purpose

# Test the code
```cmd
cat test.jp2 | ./convert jpg > output.jpg
```
or
```cmd
./convert --test
```
>Remenber its required the test.jp2 file within the core dir