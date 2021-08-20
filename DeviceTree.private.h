#pragma once
#include "DeviceTree.h"
#include <cstdint>

enum class FdtToken : uint32_t {
    // The FDT_BEGIN_NODE token marks the beginning of a node’s representation. It
    // shall be followed by the node’s unit name as extra data. The name is stored
    // as a null-terminated string, and shall include the unit address , if any.
    // The node name is followed by zeroed padding bytes, if necessary for
    // alignment, and then the next token, which may be any token except FDT_END.
    BEGIN_NODE = 0x1,
    // The FDT_END_NODE token marks the end of a node’s representation. This token
    // has no extra data; so it is followed immediately by the next token, which
    // may be any token except FDT_PROP.
    END_NODE = 0x2,
    // The FDT_PROP token marks the beginning of the representation of one property
    // in the devicetree.
    PROP = 0x3,
    // The FDT_NOP token will be ignored by any program parsing the device tree.
    // This token has no extra data; so it is followed immediately by the next
    // token, which can be any valid token. A property or node definition in the
    // tree can be overwritten with FDT_NOP tokens to remove it from the tree
    // without needing to move other sections of the tree’s representation in the
    // devicetree blob
    NOP = 0x4,
    // The FDT_END token marks the end of the structure block. There shall be only
    // one FDT_END token, and it shall be the last token in the structure block. It
    // has no extra data; so the byte immediately after the FDT_END token has
    // offset from the beginning of the structure block equal to the value of the
    // size_dt_struct field in the device tree blob header.
    END = 0x9
};

struct __attribute__((__packed__)) FdtHeader {
    // This field shall contain the value 0xd00dfeed (big-endian)
    uint32_t magic;
    // This field shall contain the total size (in bytes) of the devicetree
    // data structure. This size shall encompass all sections of the structure:
    // the header, the memory reservation block, structure block and strings
    // block, as well as any free space gaps between the blocks or after the
    // final block
    uint32_t totalsize;
    // This field shall contain the offset in bytes of the structure block from
    // the beginning of the header.
    uint32_t off_dt_struct;
    // This field shall contain the offset in bytes of the strings block from
    // the beginning of the header.
    uint32_t off_dt_strings;
    // This field shall contain the offset in bytes of the memory reservation
    // block from the beginning of the header
    uint32_t off_mem_rsvmap;
    /* This field shall contain the version of the devicetree data structure.
     * Expected to be 17 or higher
     *
     * If the version is 16, the size_dt_struct field is omitted/ignored.
     *
     * Versions 17/16 are not backwards compatible with earlier versions,
     * so we'll dismiss those earlier versions as irrelephant.
     * */
    uint32_t version;
    // Last compatible version
    uint32_t last_comp_version;
    // This field shall contain the physical ID of the system’s boot CPU. It
    // shall be identical to the physical ID given in the reg property of that
    // CPU node within the devicetree.
    uint32_t boot_cpuid_phys;
    // This field shall contain the length in bytes of the strings block
    // section of the devicetree blob.
    uint32_t size_dt_strings;
    // This field shall contain the length in bytes of the structure block
    // section of the devicetree blob
    uint32_t size_dt_struct;
};

struct FdtReserveEntry {
    uint64_t address;
    uint64_t size;
};

struct FdtProp {
    uint32_t len;
    uint32_t nameoff;
};
