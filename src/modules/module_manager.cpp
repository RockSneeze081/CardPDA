#include "module_manager.h"

namespace CardOS {

ModuleManager& ModuleManager::getInstance() {
    static ModuleManager instance;
    return instance;
}

void ModuleManager::registerModule(Module* module) {
    if (module == nullptr || module_count_ >= 8) return;
    
    for (uint8_t i = 0; i < module_count_; i++) {
        if (modules_[i]->getID() == module->getID()) {
            return;
        }
    }
    
    modules_[module_count_++] = module;
    module->init();
}

void ModuleManager::unregisterModule(ModuleID id) {
    for (uint8_t i = 0; i < module_count_; i++) {
        if (modules_[i]->getID() == id) {
            modules_[i]->destroy();
            for (uint8_t j = i; j < module_count_ - 1; j++) {
                modules_[j] = modules_[j + 1];
            }
            module_count_--;
            return;
        }
    }
}

Module* ModuleManager::getModule(ModuleID id) {
    for (uint8_t i = 0; i < module_count_; i++) {
        if (modules_[i]->getID() == id) {
            return modules_[i];
        }
    }
    return nullptr;
}

bool ModuleManager::switchModule(ModuleID id) {
    Module* target = getModule(id);
    if (target == nullptr) return false;
    
    if (current_module_ != nullptr) {
        current_module_->onExit();
        current_module_->active_ = false;
        current_module_->destroy();
    }
    
    previous_module_ = current_module_;
    current_module_ = target;
    current_module_->active_ = true;
    current_module_->init();
    current_module_->onEnter();
    
    System::getInstance().setCurrentModule(id);
    
    return true;
}

bool ModuleManager::switchModule(const char* name) {
    for (uint8_t i = 0; i < module_count_; i++) {
        if (strcmp(modules_[i]->getName(), name) == 0) {
            return switchModule(modules_[i]->getID());
        }
    }
    return false;
}

void ModuleManager::update() {
    if (current_module_ != nullptr && current_module_->active_) {
        current_module_->update();
    }
}

void ModuleManager::render() {
    if (current_module_ != nullptr && current_module_->active_) {
        current_module_->render();
    }
}

void ModuleManager::loop() {
    update();
    render();
}

}
