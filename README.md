# cs5600-malloc

Simple malloc/free implementation in C++ for CS5600

Policies supported:

- Best-fit policy
- Worst-fit policy
- First-fit policy
- Next-fit policy

Order supported:

- Insert at front
- Insert at back
- Sort based on address
- Sort based on size in descending order
- Sort based on size in ascending order

```zsh
$ make malloc

$ ./malloc -h
Usage: malloc  [OPTIONS]...
Supported options:
-s, --size=HEAPSIZE
-b, --baseAddr=BASEADDR
-p, --policy=POLICY
-o, --order=ORDER
-c, --coalesce
-a, --allocList=OPSLIST
-h, --help
```
