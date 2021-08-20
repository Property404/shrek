#include "DeviceTree.h"
#include <cassert>
#include <cstring>
#include "DeviceTree.private.h"
#include "common.h"
#include "io.h"

DeviceNode::DeviceProperty::DeviceProperty(const char* name, size_t length, const uint8_t* value) {
    this->name = name;
    this->length = length;
    this->value = value;
}

DeviceNode::DeviceNode(const uint32_t* pointer, const char* strings_block) {
    if (big_endian_to_native(*pointer) != static_cast<uint32_t>(FdtToken::BEGIN_NODE)) {
        panic("DeviceNode must be constructed at a FDT_BEGIN_NODE token");
    }
    pointer += sizeof(FdtToken::BEGIN_NODE)/sizeof(*pointer);

    this->name = reinterpret_cast<const char*>(pointer);
    pointer += ALIGN_UP(1+strlen(name), sizeof(uint32_t))/sizeof(*pointer);

    while (true) {
        const FdtToken token = static_cast<FdtToken>(big_endian_to_native((*pointer)));
        switch (token) {
            case FdtToken::PROP:
                {
                    const FdtProp* prop = (FdtProp*)(pointer+1);
                    const uint32_t len = big_endian_to_native(prop->len);
                    const uint32_t nameoff = big_endian_to_native(prop->nameoff);
                    const char* name = strings_block + nameoff;
                    pointer += 1 + (sizeof(FdtProp)/sizeof(*pointer));
                    properties.emplace_back(name, len, reinterpret_cast<const uint8_t*>(pointer));
                    pointer += ALIGN_UP(len, sizeof(*pointer))/sizeof(*pointer) - 1;
                    break;
                }
            case FdtToken::BEGIN_NODE:
                children.emplace_back(pointer, strings_block);
                pointer = static_cast<const uint32_t*>(children.back().end);
                break;
            case FdtToken::NOP:
            case FdtToken::END_NODE:
            case FdtToken::END:
                break;
            default:
                panic("Unexpected token in FDT@: 0x%x", token);
                break;
        }
        if (token == FdtToken::END_NODE || token == FdtToken::END) {
            break;
        }
        pointer++;
    }
    this->end = pointer;
}

void DeviceNode::display(int node_level) const {
    auto indent = [node_level]() {
        for (int i=0; i < node_level; i++) {
            putchar('\t');
        }
    };

    indent();

    if (strlen(name) == 0) {
        putchar('/');
    } else {
        printf("%s", name);
    }
    printf(" {\n");

    for (const auto& property : properties) {
        indent();
        if (property.getLength() > 0) {
            printf("\t%s = ", property.getName());
        } else {
            printf("\t%s;\n", property.getName());
            continue;
        }

        // Check if property is string or bytes
        // This is a bit of a wobbly heuristic
        bool is_string = true;
        for (size_t i=0; i < property.getLength(); i++) {
            const char c = property.getValue()[i];
            const char last_c = i > 0 ? property.getValue()[i-1] : '\0';
            if ((c < 0x20 || c >= 0x7F) && c != '\0') {
                is_string = false;
                break;
            }

            // Two null bytes in a row means probably not a string
            if (c == '\0' && last_c == '\0') {
                is_string = false;
                break;
            }
        }
        if (is_string) {
            putchar('"');
            for (size_t i=0; i < property.getLength(); i++) {
                const char c = property.getValue()[i];

                if (i == property.getLength() - 1 && c == '\0') {
                    break;
                }

                if (c == '\0') {
                    printf("\\0");
                } else {
                    putchar(c);
                }
            }
            putchar('"');
        } else {
            putchar('<');
            for (size_t i=0; i < property.getLength()/sizeof(uint32_t); i+=1) {
                if (i != 0) {
                    putchar(' ');
                }
                printf("0x%02x",
                    big_endian_to_native(
                    reinterpret_cast<const uint32_t*>(property.getValue())[i]));
            }
            putchar('>');
        }
        printf(";\n");
    }
    for (const auto& child : children) {
        putchar('\n');
        child.display(node_level + 1);
    }
    indent();
    printf("};\n");
}

void DeviceNode::display() const {
    display(0);
}

DeviceTree::DeviceTree(const void* dtb) {
    const FdtHeader* header = static_cast<const FdtHeader*>(dtb);
    blob = dtb;

    if (big_endian_to_native(header->magic) != 0xd00dfeed) {
        panic("This does not look like a flattened device tree!");
    }

    const char* strings_block = static_cast<const char*>(dtb) +
        big_endian_to_native(header->off_dt_strings);
    const uint64_t* mem_reserve_pointer = static_cast<const uint64_t*>(dtb) +
        big_endian_to_native(header->off_mem_rsvmap)/sizeof(uint64_t);
    const uint32_t* struct_pointer = static_cast<const uint32_t*>(dtb) +
        big_endian_to_native((header->off_dt_struct))/sizeof(*struct_pointer);

    while (true) {
        DeviceTree::MemoryRange mem_reserve;
        mem_reserve.address = reinterpret_cast<void*>(big_endian_to_native(*mem_reserve_pointer));
        mem_reserve_pointer++;
        mem_reserve.size = big_endian_to_native(*mem_reserve_pointer);
        mem_reserve_pointer++;

        if (mem_reserve.address == nullptr && mem_reserve.size == 0)
            break;

        reserved_memory_ranges.push_back(mem_reserve);
    }

    root = nullptr;
    root = allocator.construct<DeviceNode>(struct_pointer, strings_block);
}

DeviceTree::DeviceTree(DeviceTree&& other) {
    blob = other.blob;
    root = other.root;
    reserved_memory_ranges = std::move(other.reserved_memory_ranges);
    other.blob = nullptr;
    other.root = nullptr;
}

DeviceTree& DeviceTree::operator=(DeviceTree&& other) {
    blob = other.blob;
    root = other.root;
    reserved_memory_ranges = std::move(other.reserved_memory_ranges);
    other.blob = nullptr;
    other.root = nullptr;
    return *this;
}

DeviceTree::~DeviceTree() {
    blob = nullptr;
    if (root != nullptr) {
        root->~DeviceNode();
        allocator.free(root);
    }
}

void DeviceTree::display() const {
    printf("/dts-v1/;\n\n");
    for (const auto& range : reserved_memory_ranges) {
        printf("/memreserve/\t0x%016llx 0x%016llx;\n",
            reinterpret_cast<uint64_t>(range.address),
            reinterpret_cast<uint64_t>(range.size));
    }
    root->display();
}
