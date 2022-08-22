#include "rfid_debug_app_scene_start.h"

typedef enum {
    SubmenuLFTune,
    SubmenuHFField,
} SubmenuIndex;

void RfidDebugAppSceneStart::on_enter(RfidDebugApp* app, bool need_restore) {
    auto submenu = app->view_controller.get<SubmenuVM>();
    auto callback = cbc::obtain_connector(this, &RfidDebugAppSceneStart::submenu_callback);
    app->HF_field_enabled = false;

    submenu->add_item("LF Tune", SubmenuLFTune, callback, app);

    if (app->HF_field_enabled) {
        submenu->add_item("HF Field !!ENABLED!!", SubmenuHFField, callback, app);
    } else {
        submenu->add_item("HF Field", SubmenuHFField, callback, app);
    }

    if(need_restore) {
        submenu->set_selected_item(submenu_item_selected);
    }
    app->view_controller.switch_to<SubmenuVM>();
}

bool RfidDebugAppSceneStart::on_event(RfidDebugApp* app, RfidDebugApp::Event* event) {
    bool consumed = false;

    if(event->type == RfidDebugApp::EventType::MenuSelected) {
        submenu_item_selected = event->payload.menu_index;
        switch(event->payload.menu_index) {
        case SubmenuLFTune:
            app->scene_controller.switch_to_next_scene(RfidDebugApp::SceneType::LFTuneScene);
            break;
        }
        consumed = true;
    }

    return consumed;
}

void RfidDebugAppSceneStart::on_exit(RfidDebugApp* app) {
    app->view_controller.get<SubmenuVM>()->clean();
}

void RfidDebugAppSceneStart::submenu_callback(void* context, uint32_t index) {
    RfidDebugApp* app = static_cast<RfidDebugApp*>(context);

    if (index == SubmenuHFField) {
        app->HF_field_enabled = !app->HF_field_enabled;
        if (app->HF_field_enabled) {
            furi_hal_nfc_field_on();
        } else {
            furi_hal_nfc_field_off();
        }
    }

    RfidDebugApp::Event event;

    event.type = RfidDebugApp::EventType::MenuSelected;
    event.payload.menu_index = index;

    app->view_controller.send_event(&event);
}
