//------------------------------------------------------------------------------
//  Simple base skeelton app for Win32, Linux and macOS.
//------------------------------------------------------------------------------
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_time.h"
#include "sokol_glue.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "sokol_imgui.h"

static struct {
    uint64_t laptime;
    sg_pass_action pass_action;
} state;

static void init(void) {
    sg_setup(&(sg_desc){
        .context = sapp_sgcontext()
    });
    stm_setup();
    simgui_setup(&(simgui_desc_t){ 0 });

    // initial clear color
    state.pass_action = (sg_pass_action) {
        .colors[0] = { .action = SG_ACTION_CLEAR,  .val = { 0.2f, 0.2f, 0.2f, 1.0 } }
    };
}

static void frame(void) {
    const int width = sapp_width();
    const int height = sapp_height();
    const double delta_time = stm_sec(stm_round_to_common_refresh_rate(stm_laptime(&state.laptime)));
    simgui_new_frame(width, height, delta_time);

    const int toolbar_height = 30;
    const int panel_margin = 5;
    const int panel_top = toolbar_height+2*panel_margin;
    const int panel_width = 200;
    const int panel_height = height-toolbar_height-3*panel_margin;
    const int last_panel_width = width-4*panel_margin-2*panel_width;

    igSetNextWindowPos((ImVec2){0, panel_margin}, ImGuiCond_Once, (ImVec2){0,0});
    igSetNextWindowSize((ImVec2){width, toolbar_height}, ImGuiCond_Once);
    igBegin("Toolbar", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    igEnd();

    igSetNextWindowPos((ImVec2){panel_margin, panel_top}, ImGuiCond_Once, (ImVec2){0,0});
    igSetNextWindowSize((ImVec2){panel_width, panel_height}, ImGuiCond_Once);
    igBegin("Directories", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    igEnd();

    igSetNextWindowPos((ImVec2){2*panel_margin+panel_width, panel_top}, ImGuiCond_Once, (ImVec2){0,0});
    igSetNextWindowSize((ImVec2){panel_width, panel_height}, ImGuiCond_Once);
    igBegin("MDV image", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    igEnd();

    igSetNextWindowPos((ImVec2){3*panel_margin+2*panel_width, panel_top}, ImGuiCond_Once, (ImVec2){0,0});
    igSetNextWindowSize((ImVec2){last_panel_width, panel_height}, ImGuiCond_Once);
    igBegin("Preview", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    igEnd();

    sg_begin_default_pass(&state.pass_action, width, height);
    simgui_render();
    sg_end_pass();
    sg_commit();
}

static void cleanup(void) {
    simgui_shutdown();
    sg_shutdown();
}

static void event(const sapp_event* ev) {
    simgui_handle_event(ev);
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .window_title = "QLmdv 0.9.0",
        .width = 800,
        .height = 600,
    };
}
