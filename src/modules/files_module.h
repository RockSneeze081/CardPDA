#ifndef CARDOS_FILES_MODULE_H
#define CARDOS_FILES_MODULE_H

#include "module_manager.h"
#include <vector>

namespace CardOS {

struct FileEntry {
    char name[64];
    bool is_dir;
    uint32_t size;
};

class FilesModule : public Module {
public:
    void init() override;
    void update() override;
    void render() override;
    void destroy() override;

    const char* getName() const override { return "Files"; }
    ModuleID getID() const override { return ModuleID::FILES; }

private:
    void listDirectory(const char* path);
    void renderList();
    void renderViewer();
    void handleListInput();
    void handleViewerInput();
    bool navigateUp();
    void openFile(const char* filename);
    void deleteFile(const char* filename);
    void renameFile(const char* old_name, const char* new_name);

    std::vector<FileEntry> files_;
    int selected_index_ = 0;
    int scroll_offset_ = 0;
    bool viewer_mode_ = false;
    String viewer_content_;
    String viewer_filename_;
    String current_path_;
    int input_mode_ = 0;
    char input_buffer_[64];
    int input_pos_ = 0;

    static constexpr int VISIBLE_ITEMS = 8;
};

}

#endif
