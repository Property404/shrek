#include "Driver.h"
#include <algorithm>
#include "cmisc.h"

List<DriverLoader*>* drivers = nullptr;

DriverLoader::DriverLoader(const char* compatible, InitFunction init_function) {
    this->compatible = compatible;

    if (drivers == nullptr) {
        drivers = allocator.construct<List<DriverLoader*>>();
    }

    this->init_function = allocator.allocate<InitFunction>(sizeof(init_function));
    *(this->init_function) = std::move(init_function);

    drivers->push_back(this);
}

bool DriverLoader::isCompatible(const DeviceNode* node) const {
    if (!node->hasProperty("compatible")) {
        return false;
    }

    const auto& prop = node->getProperty("compatible");

    for (size_t i = 0; i < prop.getLength(); i++) {
        const char* value = prop.getValueAsString() + i;
        if (!strcmp(value, compatible)) {
            return true;
        }
        i += strlen(value);
    }

    return false;
}

void DriverLoader::loadIfCompatible(const DeviceNode* node) {
    if (isCompatible(node)) {
        (*(this->init_function))(node);
    }
}
