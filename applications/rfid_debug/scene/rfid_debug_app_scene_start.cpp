#include "rfid_debug_app_scene_start.h"

typedef enum {
    SubmenuLFTune,
    SubmenuHFField,
    SubmenuLFField
} SubmenuIndex;

void RfidDebugAppSceneStart::draw_menu(RfidDebugApp* app, int32_t selected) {
    auto submenu = app->view_controller.get<SubmenuVM>();
    auto callback = cbc::obtain_connector(this, &RfidDebugAppSceneStart::submenu_callback);

    submenu->clean();

    submenu->add_item("LF Tune", SubmenuLFTune, callback, app);

    if (app->HF_field_enabled) {
        submenu->add_item("HF Field    !! ENABLED !!", SubmenuHFField, callback, app);
    } else {
        submenu->add_item("HF Field", SubmenuHFField, callback, app);
    }

    if (app->LF_field_enabled) {
        submenu->add_item("LF Field    !! ENABLED !!", SubmenuLFField, callback, app);
    } else {
        submenu->add_item("LF Field", SubmenuLFField, callback, app);
    }

    if (selected != -1) {
        submenu->set_selected_item(selected);
    }
}

void RfidDebugAppSceneStart::on_enter(RfidDebugApp* app, bool need_restore) {
    app->notification = (NotificationApp*)furi_record_open(RECORD_NOTIFICATION);
    app->HF_field_enabled = false;

    draw_menu(app, need_restore ? submenu_item_selected : -1);

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
    furi_hal_nfc_field_off();
    app->worker.stop_read();
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
        } else if (index == SubmenuLFField) {
    } else {
        RfidDebugApp::Event event;

        event.type = RfidDebugApp::EventType::MenuSelected;
        event.payload.menu_index = index;

        app->view_controller.send_event(&event);
    }
}
