#pragma once
#include "List.h"

/// Device tree node
class DeviceNode {
 public:
    /// Device tree property.
    class DeviceProperty {
        const char* name;
        size_t length;
        const uint8_t* value;

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

        /// @return The value of the property as bytes.
        const uint8_t* getValue() const {
            return value;
        }

        /// @return The length of the property's value.
        size_t getLength() const {
            return length;
        }
    };


 private:
    const char* name;
    List<DeviceProperty> properties;
    List<DeviceNode> children;
    // Pointer to this Node's FDT_END_NODE
    const void* end;

    // Display the node as the node_level'th node in a tree.
    void display(int node_level) const;

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

    /// Display the node
    void display() const;
};

class DeviceTree {
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
};
