#ifndef CARDOS_MODULE_MANAGER_H
#define CARDOS_MODULE_MANAGER_H

#include "../core/system.h"

namespace CardOS {

class Module {
public:
    virtual ~Module() = default;
    
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual void destroy() = 0;
    
    virtual const char* getName() const = 0;
    virtual ModuleID getID() const = 0;
    
    virtual void onEnter() {}
    virtual void onExit() {}

protected:
    bool active_ = false;
    friend class ModuleManager;
};

class ModuleManager {
public:
    static ModuleManager& getInstance();
    
    void registerModule(Module* module);
    void unregisterModule(ModuleID id);
    Module* getModule(ModuleID id);
    Module* getCurrentModule() const { return current_module_; }
    
    bool switchModule(ModuleID id);
    bool switchModule(const char* name);
    
    void update();
    void render();
    
    void loop();

private:
    ModuleManager() : current_module_(nullptr), previous_module_(nullptr) {}
    ~ModuleManager() = default;
    ModuleManager(const ModuleManager&) = delete;
    ModuleManager& operator=(const ModuleManager&) = delete;
    
    Module* current_module_;
    Module* previous_module_;
    Module* modules_[8];
    uint8_t module_count_ = 0;
};

}

#endif
