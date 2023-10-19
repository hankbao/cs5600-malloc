// main.cc
// Simple malloc/free implementation for CS5600
// Author: Hank Bao

#include <cstdio>
#include <memory>
#include <set>
#include <sstream>
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

auto policy_to_str(Policy policy) -> std::string {
    switch (policy) {
        case Policy::BestFit:
            return "BEST";
        case Policy::WorstFit:
            return "WORST";
        case Policy::FirstFit:
            return "FIRST";
        case Policy::NextFit:
            return "NEXT";
    }
}

auto order_to_str(ListOrder order) -> std::string {
    switch (order) {
        case ListOrder::AddrSort:
            return "ADDRSORT";
        case ListOrder::SizeSortAsc:
            return "SIZESORT+";
        case ListOrder::SizeSortDesc:
            return "SIZESORT-";
        case ListOrder::InsertFront:
            return "INSERT-FRONT";
        case ListOrder::InsertBack:
            return "INSERT-BACK";
    }
}

auto op_to_str(Op op) -> std::string {
    switch (op) {
        case Op::Alloc:
            return "+";
        case Op::Free:
            return "-";
    }
}

auto ops_to_str(const std::vector<MemOp>& ops) -> std::string {
    std::stringstream ss;
    auto it = ops.cbegin();
    ss << op_to_str(it->op()) << it->num();
    ++it;
    for (; it != ops.cend(); ++it) {
        ss << "," << op_to_str(it->op()) << it->num();
    }
    return ss.str();
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
    auto oplist = std::vector<MemOp>{};

    auto strlist = split_string(ops, ",");
    for (const auto& op : strlist) {
        // num is size when allocating, and index of allocated chunk when freeing
        size_t num = std::stoi(op.substr(1));
        switch (op[0]) {
            case '+':
                if (num <= 0) {
                    std::fprintf(stderr, "Invalid mem-op size: %s\n", op.c_str());
                    print_usage(true);
                }
                oplist.push_back(MemOp(Op::Alloc, num));
                break;
            case '-':
                oplist.push_back(MemOp(Op::Free, num));
                break;
            default:
                std::fprintf(stderr, "Invalid mem-op: %s\n", op.c_str());
                print_usage(true);
        }
    }

    return oplist;
}

auto exec_memops(const std::vector<MemOp>& ops, std::unique_ptr<Allocator> allocator) -> void {
    if (ops.empty()) {
        std::fprintf(stderr, "Invalid mem-ops: no op found.\n");
        print_usage(true);
    }

    std::vector<Chunk> allocated{};
    std::set<size_t> freed{};

    for (const auto& op : ops) {
        switch (op.op()) {
            case Op::Alloc: {
                auto c = allocator->malloc(op.num());

                // allocation failed if null chunk returned
                if (c.is_null()) {
                    std::fprintf(stderr, "Failed to allocate %lu bytes\n", op.num());
                    ::exit(EXIT_FAILURE);
                }

                auto searched = allocator->last_searched();
                std::printf("ptr[%lu] = Alloc(%lu) returned %lu (searched %lu %s)\n",
                            allocated.size(), c.size(), c.base(), searched,
                            searched > 1 ? "elements" : "element");
                allocated.push_back(c);
            } break;

            case Op::Free: {
                auto idx = op.num();
                if (idx >= allocated.size()) {
                    std::fprintf(stderr, "Invalid free index: %lu\n", op.num());
                    ::exit(EXIT_FAILURE);
                }

                // double-free detection
                auto it = freed.find(idx);
                if (it != freed.end()) {
                    std::fprintf(stderr, "Double-free detected on index: %lu\n", op.num());
                    ::exit(EXIT_FAILURE);
                }

                auto c = allocated.at(idx);
                std::printf("Free(ptr[%lu]) at %lu\n", idx, c.base());
                allocator->free(c);

                // mark the index as freed
                freed.insert(idx);
            } break;
        }

        allocator->print_status();
    }
}

auto main(int argc, char** argv) -> int {
    int opt;
    size_t heap_size = 100;
    size_t base_addr = 1000;
    Policy policy = Policy::BestFit;
    ListOrder order = ListOrder::AddrSort;
    bool coalesce = false;
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

    std::printf("base_addr: %lu\n", base_addr);
    std::printf("heap_size: %lu\n", heap_size);
    std::printf("policy: %s\n", policy_to_str(policy).c_str());
    std::printf("order: %s\n", order_to_str(order).c_str());
    std::printf("coalesce: %s\n", coalesce ? "true" : "false");
    std::printf("mem-ops: %s\n", ops_to_str(ops).c_str());
    std::puts("");

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
    return EXIT_SUCCESS;
}
