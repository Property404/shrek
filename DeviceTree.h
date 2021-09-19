#pragma once
#include "List.h"
#include "Slice.h"
#include <iterator>

// Used for our ScalarTransform in our slices
template<typename T>
static std::function<T(T)> _big_endian_to_native_as_std_function =
    big_endian_to_native<T>;

/// Device tree node
class DeviceNode final {
 public:
    /// Device tree property.
    class DeviceProperty final {
        const char* name;
        size_t length;
        const void* value;


     public:
        DeviceProperty(const char* name, size_t length, const uint8_t* value);
        DeviceProperty(const DeviceProperty&) = delete;
        DeviceProperty operator=(const DeviceProperty&) = delete;
        DeviceProperty(DeviceProperty&& other) = default;
        DeviceProperty& operator=(DeviceProperty&& other) = default;

        /// @return The name of the property as a null-terminated string.
        const char* getName() const {
            return name;
        }

        const char* getValueAsString() const {
            return static_cast<const char*>(value);
        }

        /// @return The value as a native-endian contiguous array slice.
        template<typename U, typename T = const U>
        Slice<T, &_big_endian_to_native_as_std_function<T>> getValueAsSlice() const {
            if (length % sizeof(T) != 0) {
                panic("DeviceProperty: Inappropriately sized slice: %x bytes", length);
            }
            return Slice<T, &_big_endian_to_native_as_std_function<T>>
                (static_cast<T*>(value), length/sizeof(T));
        }

        /// @return The value as a native-endian scalar value
        template <typename T>
        T getValueAsScalar() const {
            return big_endian_to_native(*(static_cast<const T*>(value)));
        }

        /// @return The length of the property's value.
        size_t getLength() const {
            return length;
        }
    };


 private:
    const char* name;
    DeviceNode* parent = nullptr;
    List<DeviceProperty> properties;
    List<DeviceNode> children;
    // Pointer to this Node's FDT_END_NODE
    const void* end;

    // Display the node as the node_level'th node in a tree.
    void display(int node_level) const;

    // Get the #address-cells property
    size_t getNumAddressCells() const;

    // Get the #size-cells property
    size_t getNumSizeCells() const;

    // Map child's address into CPU address space
    // Recursive companion function to getBaseAddress()
    uint64_t translateAddress(uint64_t address) const;

 public:
    /// Constructor
    ///
    /// @param pointer Raw pointer to the node in the DTB.
    ///
    /// @return The constructed DeviceNode.
    DeviceNode(const uint32_t* pointer, const char* strings_block);
    DeviceNode(const DeviceNode&) = delete;
    DeviceNode operator=(const DeviceNode&) = delete;
    DeviceNode(DeviceNode&&) = default;
    DeviceNode& operator=(DeviceNode&&) = default;

    /// @return The node name as a null-terminated string.
    const char* getName() const {
        return name;
    }

    /// @return The node properties.
    const List<DeviceProperty>& getProperties() const {
        return properties;
    }

    /// @return Child nodes.
    const List<DeviceNode>& getChildren() const {
        return children;
    }

    /// @return The base address of the registers.
    void* getBaseAddress() const;

    /// @return True if property `name` exists, otherwise false.
    bool hasProperty(const char* name) const;

    /// @return The property associated with `name` if it exists, otherwise panic.
    const DeviceProperty& getProperty(const char* name) const;

    /// @return True if node `name` exists underneath this one, otherwise false.
    bool hasNode(const char* name) const;

    /// @return The node associated with `name` if it exists, otherwise panic.
    const DeviceNode& getNode(const char* name) const;

    /// Display the node
    void display() const;
};

class DeviceTree final {
 public:
    struct MemoryRange {
        void* address;
        uint64_t size;
    };

 private:
    // Beginning of device tree blob.
    const void* blob;
    DeviceNode* root;
    List<MemoryRange> reserved_memory_ranges;

 public:
    /// Construct a DeviceTree out of a raw device tree blob.
    ///
    /// @param blob The device tree blob.
    explicit DeviceTree(const void* blob);

    DeviceTree(const DeviceTree&) = delete;
    DeviceTree operator=(const DeviceTree&) = delete;
    DeviceTree(DeviceTree&& other);
    DeviceTree& operator=(DeviceTree&& other);
    ~DeviceTree();

    /// Display the tree as dtc would.
    void display() const;

    /// @return The root node
    DeviceNode& getRoot() const {
        if (root == nullptr) {
            panic("DeviceTree has null root!");
        }
        return *root;
    }

    /// @return Unparsed device tree blob
    const void* getBlobAddress() const {
        return blob;
    }

    /// Iterator over DeviceNode's.
    struct Iterator final {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using NodePointer = const DeviceNode*;
        using NodeReference = const DeviceNode&;

        explicit Iterator(NodePointer pointer);

        NodeReference operator*() const {
            if (pointer == nullptr) {
                panic("Can't dereference null iterator!");
            }
            return *pointer;
        }

        Iterator& operator++();

        friend bool operator==(Iterator& a, const Iterator& b) {
            return a.pointer == b.pointer;
        }

        friend bool operator!=(Iterator& a, const Iterator& b) {
            return a.pointer != b.pointer;
        }

     private:
            NodePointer pointer;
            List<NodePointer> stack;
    };

    Iterator begin() const {
        return Iterator(root);
    }

    Iterator end() const {
        return Iterator(nullptr);
    }
};
