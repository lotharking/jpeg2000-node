# Compile
```c
gcc convert.c -o convert -static -lopenjp2 -lm
```
```cmd
cat test.jp2 | ./convert jpg > output.jpg
```