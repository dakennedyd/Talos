
# Talos

A simple UCI Chess(WIP) engine written in C++.
compile it with:

```
g++ -std=c++17 -g -Wall -msse2 init.cpp Move.cpp Chessboard.cpp Engine.cpp  main.cpp  -o ./talos
```

only tested on Ubuntu GNU/Linux 16.04 but should also work on Windows with minor modifications