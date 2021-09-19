/*
 * Shrek's driver model.
 *
 * Drivers are divided into two parts:
 *  * A `DriverLoader` object that initializes the driver and (optionally?)
 *    instantiates a "coupling". These are initialized during dynamic
 *    initialization. Registration takes place as part of the initialization.
 *  * A `DeviceCoupling`-derived object that acts as interface to hardware.
 *    Abstract classes specific to the type of device are derived from the
 *    abstract DeviceCoupling classes. Each driver will have a concrete class
 *    derived from one of those abstract child classes. The word "coupling" was
 *    chosen as an alternative to "interface", because the latter has a common
 *    meaning in OOP.
 */

#pragma once
#include "DeviceTree.h"
#include <functional>

// https://stackoverflow.com/questions/21143835/can-i-implement-an-autonomous-self-member-type-in-c
template <typename...Ts>
class DeviceCoupling;

template <typename X, typename...Ts>
class DeviceCoupling<X, Ts...> : public Ts... {
protected:
    typedef X self;

    static self* accessInstance(self* val) {
        static self* instance = nullptr;
        if (val != nullptr) {
            if (instance != nullptr) {
                panic("Driver already initialized");
            }
            instance = val;
        }
        return instance;
    }
public:
    static void setInstance(self* i) {
        accessInstance(i);
    }

    static self* getInstance() {
        self* instance = accessInstance(nullptr);
        if (instance == nullptr) {
            panic("Driver not initialized");
        }
        return instance;
    }

    static bool initialized() {
        return accessInstance(nullptr) != nullptr;
    }
};

#define DEVICE_COUPLING_INTERFACE(X) X : public DeviceCoupling<X>

#define REGISTER_DRIVER(compatible, Coupling)\
    static DriverLoader _driver_(compatible, [](const DeviceNode* node) {\
    if (Coupling::initialized()) { return;}\
    Coupling* coupling_instance = allocator.construct< Coupling >(node);\
    Coupling::setInstance(coupling_instance);\
    });

// A Shrek device driver.
class DriverLoader final {
 public:
    using InitFunction = std::function<void(const DeviceNode* node)>;

    // Create and register this driver
    DriverLoader(const char* compatible, InitFunction init_function);

    /// Initialize this driver if compatible and create its interface.
    void loadIfCompatible(const DeviceNode* node);

 private:
    InitFunction* init_function;

    /// Check if this driver is compatible with a device tree node
    bool isCompatible(const DeviceNode* node) const;

    // Related to the "compatible" property of device tree nodes.
    const char* compatible;
};

// List of all registered drivers
extern List<DriverLoader*>* drivers;
