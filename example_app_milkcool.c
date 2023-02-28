#include <furi.h>        // Core libraries
#include <furi_hal.h>    // Hardware libraries

#include <gui/gui.h>     // GUI
#include <input/input.h> // Input

// Screen is 128x64 px
static void app_draw_callback(Canvas* canvas, void* ctx) {
	// Canvas type is defined at https://github.com/flipperdevices/flipperzero-firmware/blob/dev/applications/services/gui/canvas_i.h#L13
	// Context can be anything
	UNUSED(ctx); // We don't use the context variable here

	canvas_clear(canvas); // Clearing the canvas
	canvas_set_font(canvas, FontPrimary); // Setting the font
	// Can either be FontPrimary, FontSecondary, FontKeyboard or FontBigNumbers
	canvas_draw_str(canvas, 0, 12, "Hello World!"); // Drawing "Hello World!" at (0;12)
}

// This function puts the input events in a queue
static void app_input_callback(InputEvent* input_event, void* ctx) {
	// InputEvent is defined at https://github.com/flipperdevices/flipperzero-firmware/blob/dev/applications/services/input/input.h#L29
	furi_assert(ctx); // Making sure that context is non-empty

	FuriMessageQueue* event_queue = ctx; // Getting the event queue from context argument
	furi_message_queue_put(event_queue, input_event, FuriWaitForever); // Pushing the input event into the queue
}

int32_t example_app_milkcool_main(void* p) {
	UNUSED(p);
	
	FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent)); // Allocating memory for the event queue

	// Configure view port
	ViewPort* view_port = view_port_alloc(); // Allocationg memory for the view port
	// ViewPort is defined at https://github.com/flipperdevices/flipperzero-firmware/blob/dev/applications/services/gui/view_port_i.h#L11
	view_port_draw_callback_set(view_port, app_draw_callback, view_port); // Setting the draw allbacl
	view_port_input_callback_set(view_port, app_input_callback, event_queue); // Setting the input callback

	// Register view port in GUI
	Gui* gui = furi_record_open(RECORD_GUI); // Opening the RECORD_GUI record
	gui_add_view_port(gui, view_port, GuiLayerFullscreen); // Adding the view port

	InputEvent event; // Creating the event variable

	bool running = true; // Creating the "running" variable
	while(running) { // While the "running" variable is set to true, listen for input events
		if(furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) { // If there is a new eveent in the queue
			// Key press event types: https://github.com/flipperdevices/flipperzero-firmware/blob/dev/applications/services/input/input.h#L19
			// Keys: https://github.com/flipperdevices/flipperzero-firmware/blob/dev/firmware/targets/f7/furi_hal/furi_hal_resources.h#L16
			if(event.type == InputTypePress && event.key == InputKeyBack) running = false; // Exiting the loop if the back button was pressed
		}
		view_port_update(view_port); // Updating the view port
	}

	view_port_enabled_set(view_port, false); // Disabling view port
	gui_remove_view_port(gui, view_port); // Removing the view port from the gui variable
	view_port_free(view_port); // Freeing the memory allocated for the view port
	furi_message_queue_free(event_queue); // Freeing the memory allocated for the event queue

	furi_record_close(RECORD_GUI); // Closing the GUI record

	return 0;
}
