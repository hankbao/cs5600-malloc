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
Usage: malloc [OPTIONS]...

Supported options:
-s, --size=HEAPSIZE
	size of the heap
-b, --base=BASEADDR
	base address of heap
-p, --policy=POLICY
	list search (BEST, WORST, FIRST, NEXT)
-o, --order=ORDER
	list order (ADDRSORT, SIZESORT+, SIZESORT-, INSERT-FRONT, INSERT-BACK)
-c, --coalesce
	coalesce the free list
-a, --memops=OPSLIST
	list of ops (+10,-0,etc)
-h, --help
	print usage message and exit
```
