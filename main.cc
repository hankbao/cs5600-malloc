// main.cc
// Simple malloc/free implementation for CS5600
// Author: Hank Bao

#include <cstdio>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <getopt.h>

#include "allocator.h"
#include "allocator_best.h"
#include "allocator_worst.h"
#include "allocator_first.h"
#include "allocator_next.h"
#include "chunk.h"

enum class Policy {
    BestFit,
    WorstFit,
    FirstFit,
    NextFit,
};

enum class Op {
    Alloc,
    Free,
};

class MemOp {
   public:
    MemOp(Op op, size_t num) : op_{op}, num_{num} {};
    ~MemOp() = default;

    auto op() const -> Op { return op_; }

    // num is size when allocating, and index of allocated chunk when freeing
    auto num() const -> size_t { return num_; }

   private:
    Op op_;
    size_t num_;
};

[[noreturn]] auto print_usage(bool onerror) -> void {
    std::puts("Usage: malloc [OPTIONS]...\n");
    std::puts("Supported options:");
    std::puts("-s, --size=HEAPSIZE\n\tsize of the heap");
    std::puts("-b, --base=BASEADDR\n\tbase address of heap");
    std::puts("-p, --policy=POLICY\n\tlist search (BEST, WORST, FIRST, NEXT)");
    std::puts(
        "-o, --order=ORDER\n\tlist order (ADDRSORT, SIZESORT+, SIZESORT-, INSERT-FRONT, INSERT-BACK)");
    std::puts("-c, --coalesce\n\tcoalesce the free list");
    std::puts("-a, --memops=OPSLIST\n\tlist of ops (+10,-0,etc)");
    std::puts("-h, --help\n\tprint usage message and exit");

    ::exit(onerror ? EXIT_FAILURE : EXIT_SUCCESS);
}

auto split_string(const std::string& s, const std::string& delimiter) -> std::vector<std::string> {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = s.find(delimiter);

    while (end != std::string::npos) {
        result.push_back(s.substr(start, end - start));
        start = end + delimiter.length();
        end = s.find(delimiter, start);
    }

    result.push_back(s.substr(start, end));
    return result;
}

auto parse_heap_size(const std::string& str) -> size_t {
    size_t heap_size = std::stoi(str);
    if (heap_size <= 0) {
        std::fprintf(stderr, "Invalid heap size: %s\n", str.c_str());
        print_usage(true);
    }
    return heap_size;
}

auto parse_base_addr(const std::string& str) -> size_t {
    size_t base_addr = std::stoi(str);
    if (base_addr <= 0) {
        std::fprintf(stderr, "Invalid base address: %s\n", str.c_str());
        print_usage(true);
    }
    return base_addr;
}

auto parse_policy(const std::string& policy) -> Policy {
    if (policy == "BEST") {
        return Policy::BestFit;
    } else if (policy == "WORST") {
        return Policy::WorstFit;
    } else if (policy == "FIRST") {
        return Policy::FirstFit;
    } else if (policy == "NEXT") {
        return Policy::NextFit;
    } else {
        std::fprintf(stderr, "Invalid policy: %s\n", policy.c_str());
        print_usage(true);
    }
}

auto parse_order(const std::string& order) -> ListOrder {
    if (order == "ADDRSORT") {
        return ListOrder::AddrSort;
    } else if (order == "SIZESORT+") {
        return ListOrder::SizeSortAsc;
    } else if (order == "SIZESORT-") {
        return ListOrder::SizeSortDesc;
    } else if (order == "INSERT-FRONT") {
        return ListOrder::InsertFront;
    } else if (order == "INSERT-BACK") {
        return ListOrder::InsertBack;
    } else {
        std::fprintf(stderr, "Invalid order: %s\n", order.c_str());
        print_usage(true);
    }
}

auto parse_ops(const std::string& ops) -> std::vector<MemOp> {
    if (ops.empty()) {
        std::fprintf(stderr, "Invalid ops list: empty\n");
        print_usage(true);
    }

    auto opslist = std::vector<MemOp>{};

    auto strlist = split_string(ops, ",");
    for (const auto& op : strlist) {
        size_t size = std::stoi(op.substr(1));
        if (size <= 0) {
            std::fprintf(stderr, "Invalid mem-op size: %s\n", op.c_str());
            print_usage(true);
        }

        switch (op[0]) {
            case '+':
                opslist.push_back(MemOp(Op::Alloc, size));
                break;
            case '-':
                opslist.push_back(MemOp(Op::Free, size));
                break;
            default:
                std::fprintf(stderr, "Invalid mem-op: %s\n", op.c_str());
                print_usage(true);
        }
    }

    return opslist;
}

auto exec_memops(const std::vector<MemOp>& ops, std::unique_ptr<Allocator> allocator) -> void {
    std::vector<Chunk> allocated{};

    for (const auto& op : ops) {
        switch (op.op()) {
            case Op::Alloc: {
                auto c = allocator->malloc(op.num());
                if (c.is_null()) {
                    std::fprintf(stderr, "Failed to allocate %zu bytes\n", op.num());
                    ::exit(EXIT_FAILURE);
                }
                allocated.push_back(c);

                // TODO: print
            } break;

            case Op::Free: {
                auto c = allocated.at(op.num());
                allocator->free(c);

                // TODO: print
            } break;
        }
    }
}

auto main(int argc, char** argv) -> int {
    int opt;
    size_t heap_size = 100;
    size_t base_addr = 1000;
    bool coalesce = false;
    Policy policy = Policy::BestFit;
    ListOrder order = ListOrder::AddrSort;
    std::vector<MemOp> ops{};

    struct option long_options[] = {
        {"size", required_argument, nullptr, 's'},
        {"base", required_argument, nullptr, 'b'},
        {"policy", required_argument, nullptr, 'p'},
        {"order", required_argument, nullptr, 'o'},
        {"coalesce", no_argument, 0, 'c'},
        {"memops", required_argument, nullptr, 'a'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0}};

    while ((opt = getopt_long(argc, argv, "s:b:p:o:ca:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_usage(false);
                break;
            case 's':
                heap_size = parse_heap_size(optarg);
                break;
            case 'b':
                base_addr = parse_base_addr(optarg);
                break;
            case 'p':
                policy = parse_policy(optarg);
                break;
            case 'o':
                order = parse_order(optarg);
                break;
            case 'c':
                coalesce = true;
                break;
            case 'a':
                ops = parse_ops(optarg);
                break;
            default:
                std::fprintf(stderr, "Unknown option: %c\n", optopt);
                print_usage(true);
        }
    }

    std::unique_ptr<Allocator> allocator = nullptr;
    switch (policy) {
        case Policy::BestFit:
            allocator = std::make_unique<AllocatorBest>(base_addr, heap_size, coalesce, order);
            break;

        case Policy::WorstFit:
            allocator = std::make_unique<AllocatorWorst>(base_addr, heap_size, coalesce, order);

        case Policy::FirstFit:
            allocator = std::make_unique<AllocatorFirst>(base_addr, heap_size, coalesce, order);
            break;

        case Policy::NextFit:
            allocator = std::make_unique<AllocatorNext>(base_addr, heap_size, coalesce, order);
            break;
    }

    exec_memops(ops, std::forward<std::unique_ptr<Allocator>>(allocator));

    return 0;
}
