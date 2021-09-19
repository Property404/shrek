#include "DeviceTree.h"
#include <cassert>
#include <cstring>
#include "DeviceTree.private.h"
#include "common.h"
#include "cmisc.h"
#include "io.h"

constexpr size_t DEFAULT_NUM_SIZE_CELLS = 1;
constexpr size_t DEFAULT_NUM_ADDRESS_CELLS = 2;

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

    this->parent = nullptr;
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
                children.back().parent = this;
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

bool DeviceNode::hasProperty(const char* name) const {
    for (const auto& prop : properties) {
        if (!strcmp(prop.getName(), name)) {
            return true;
        }
    }
    return false;
}

const DeviceNode::DeviceProperty& DeviceNode::getProperty(const char* name) const {
    for (const auto& prop : properties) {
        if (!strcmp(prop.getName(), name)) {
            return prop;
        }
    }
    panic("Node '%s' has no property with name '%s'", this->name, name);
    halt();
}

bool DeviceNode::hasNode(const char* name) const {
    for (const auto& child : children) {
        if (!strcmp(child.getName(), name) || child.hasNode(name)) {
            return true;
        }
    }
    return false;
}

const DeviceNode& DeviceNode::getNode(const char* name) const {
    for (const auto& child : children) {
        if (!strcmp(child.getName(), name)) {
            return child;
        }
        if (child.hasNode(name)) {
            return child.getNode(name);
        }
    }
    panic("Node '%s' has no child with name '%s'", this->name, name);
    halt();
}

size_t DeviceNode::getNumAddressCells() const {
    if (hasProperty("#address-cells")) {
        const auto num_address_cells =  getProperty("#address-cells").getValueAsScalar<uint32_t>();
        if (num_address_cells < 1) {
            panic("#address-cells cannot be less than 1");
        }
        if (num_address_cells > 2) {
            panic("This Device Tree implementation can't handle #address-cells > 2");
        }
        return num_address_cells;
    } else {
        return DEFAULT_NUM_ADDRESS_CELLS;
    }
}

size_t DeviceNode::getNumSizeCells() const {
    if (hasProperty("#size-cells")) {
        const auto num_size_cells = getProperty("#size-cells").getValueAsScalar<uint32_t>();
        if (num_size_cells < 1) {
            panic("#size-cells cannot be less than 1");
        }
        if (num_size_cells > 2) {
            panic("This Device Tree implementation can't handle #size-cells > 2 (%x)",
                num_size_cells);
        }
        return num_size_cells;
    } else {
        return DEFAULT_NUM_SIZE_CELLS;
    }
}

uint64_t DeviceNode::translateAddress(uint64_t address) const {
    const auto num_size_cells = getNumSizeCells();
    const auto num_child_address_cells = getNumAddressCells();
    const auto num_parent_address_cells =
        parent == nullptr?DEFAULT_NUM_ADDRESS_CELLS:parent->getNumAddressCells();

    uint64_t return_value = address;

    if (hasProperty("ranges")) {
        const auto ranges = getProperty("ranges").getValueAsSlice<uint32_t>();

        size_t index = 0;
        while (index < ranges.size()) {
            uint64_t child_base = 0;
            for (size_t i = 0; i < num_child_address_cells; i++) {
                child_base += static_cast<uint64_t>
                    (ranges[index++]) << ((num_child_address_cells - 1 - i)*32);
            }

            uint64_t parent_base = 0;
            for (size_t i = 0; i < num_parent_address_cells; i++) {
                parent_base += static_cast<uint64_t>
                    (ranges[index++]) << ((num_parent_address_cells - 1 - i)*32);
            }

            uint64_t size = 0;
            for (size_t i = 0; i < num_size_cells; i++) {
                size = static_cast<uint64_t>(ranges[index++]) << ((num_size_cells - 1  - i)*32);
            }

            if (address >= child_base && address <= child_base + size) {
                return_value = address - child_base + parent_base;
                break;
            }
        }
    }

    if (this->parent == nullptr) {
        return return_value;
    } else {
        return this->parent->translateAddress(return_value);
    }
}

void* DeviceNode::getBaseAddress() const {
    if (this->parent == nullptr) {
        panic("Can't get base address of root");
    }

    const auto num_address_cells = parent->getNumAddressCells();
    const auto registers = getProperty("reg").getValueAsSlice<uint32_t>();

    if (registers.size() < num_address_cells) {
        panic("Can't access full address from `reg` property");
    }

    uint64_t  address;
    if (num_address_cells == 1) {
        address = registers[0];
    } else {
        if (registers[0] != 0) {
            panic("Device tree implementation not equipped to deal with high addresses in reg");
        }
        address = registers[1];
    }

    uint64_t rv = this->parent->translateAddress(address);

    if (rv > 0xFFFFFFFFULL) {
        panic("Can't return high address as base!");
    }

    return reinterpret_cast<void*>(static_cast<uintptr_t>(rv));
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

        const auto char_slice = property.getValueAsSlice<const char>();

        // Check if property is string or bytes
        // This is a bit of a wobbly heuristic
        bool is_string = true;
        for (size_t i=0; i < property.getLength(); i++) {
            const char c = char_slice[i];
            const char last_c = i > 0 ? char_slice[i-1] : '\0';
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
                const char c = char_slice[i];

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
                    big_endian_to_native(property.getValueAsSlice<const uint32_t>()[i]));
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

DeviceTree::Iterator::Iterator(NodePointer pointer) {
    this->pointer = pointer;

    if (pointer == nullptr) {
        return;
    }

    for (const auto& child : pointer->getChildren()) {
        stack.push_back(&child);
    }
}

DeviceTree::Iterator& DeviceTree::Iterator::operator++() {
    if (stack.size() == 0) {
        pointer = nullptr;
        return *this;
    }
    pointer = stack.pop_front();
    for (const auto& child : pointer->getChildren()) {
        stack.push_back(&child);
    }
    return *this;
}
