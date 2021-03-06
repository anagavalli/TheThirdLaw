//
//  RDInput.cpp
//  Rocket Demo
//
//  This input controller is primarily designed for keyboard control.  On mobile
//  you will notice that we use gestures to emulate keyboard commands. They even
//  use the same variables (though we need other variables for internal keyboard
//  emulation).  This simplifies our design quite a bit.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 1/10/17
//
#include "RDInput.h"

using namespace cugl;

#pragma mark Input Constants

/** The key to use for reseting the game */
#define RESET_KEY KeyCode::R
/** The key for toggling the debug display */
#define DEBUG_KEY KeyCode::D
/** The key for exitting the game */
#define EXIT_KEY  KeyCode::ESCAPE
/* The pause key*/
#define PAUSE_KEY KeyCode::P

/** How fast a double click must be in milliseconds */
#define EVENT_DOUBLE_CLICK  400
/** How far we must swipe left or right for a gesture */
#define EVENT_SWIPE_LENGTH  100000000
/** How fast we must swipe left or right for a gesture */
#define EVENT_SWIPE_TIME   1000
/** How far we must turn the tablet for the accelerometer to register */
#define EVENT_ACCEL_THRESH  M_PI/10.0f
/** The key for the event handlers */
#define LISTENER_KEY        1


#pragma mark -
#pragma mark Input Controller
/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
RocketInput::RocketInput() :
_active(false),
_resetPressed(false),
_debugPressed(false),
_exitPressed(false),
_pausePressed(false),
_keyUp(false),
_keyDown(false),
_keyReset(false),
_keyDebug(false),
_keyExit(false),
_horizontal(0.0f),
_vertical(0.0f) {
}

/**
 * Deactivates this input controller, releasing all listeners.
 *
 * This method will not dispose of the input controller. It can be reused
 * once it is reinitialized.
 */
void RocketInput::dispose() {
    if (_active) {
#ifndef CU_TOUCH_SCREEN
        Input::deactivate<Keyboard>();

		// PBB cleanup mouse listeners
		Mouse* mouse = Input::get<Mouse>();
		mouse->removePressListener(LISTENER_KEY);
		mouse->removeReleaseListener(LISTENER_KEY);
#else
        Input::deactivate<Accelerometer>();
        Touchscreen* touch = Input::get<Touchscreen>();
        touch->removeBeginListener(LISTENER_KEY);
        touch->removeEndListener(LISTENER_KEY);
#endif
        _active = false;
    }
}

/**
 * Initializes the input control for the given drawing scale.
 *
 * This method works like a proper constructor, initializing the input
 * controller and allocating memory.  However, it still does not activate
 * the listeners.  You must call start() do that.
 *
 * @return true if the controller was initialized successfully
 */
bool RocketInput::init() {
    _timestamp.mark();
    bool success = true;
    
	// PBB: Get mouse (or touch which should be below See polydemo/PDInput.cpp)
	_touchProc = false;

    // Only process keyboard on desktop
#ifndef CU_TOUCH_SCREEN
    success = Input::activate<Keyboard>();

	Mouse* mouse = Input::get<Mouse>();

	assert(mouse->addPressListener(LISTENER_KEY, [=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
		this->mouseBeganCB(event, focus);
	}));
	assert(mouse->addReleaseListener(LISTENER_KEY, [=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
		this->mouseEndedCB(event, focus);
	}));

#else
    success = Input::activate<Accelerometer>();
    Touchscreen* touch = Input::get<Touchscreen>();
    touch->addBeginListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, bool focus) {
        this->touchBeganCB(event,focus);
    });
    touch->addEndListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, bool focus) {
        this->touchEndedCB(event,focus);
    });
#endif
    _active = success;
    return success;
}



/**
 * Processes the currently cached inputs.
 *
 * This method is used to to poll the current input state.  This will poll the
 * keyboad and accelerometer.
 *
 * This method also gathers the delta difference in the touches. Depending on
 * the OS, we may see multiple updates of the same touch in a single animation
 * frame, so we need to accumulate all of the data together.
 */
void RocketInput::update(float dt) {
	int left = false;
	int rght = false;
	int up = false;
	int down = false;

#ifndef CU_TOUCH_SCREEN
	// DESKTOP CONTROLS
	Keyboard* keys = Input::get<Keyboard>();

	// Map "keyboard" events to the current frame boundary
	_keyReset = keys->keyPressed(RESET_KEY);
	_keyDebug = keys->keyPressed(DEBUG_KEY); //THIS MAY HAVE CAUSED THE PROBLEM OF THE GAME
	_keyExit = keys->keyPressed(EXIT_KEY);
	_pausePressed = keys->keyPressed(PAUSE_KEY);

	left = keys->keyDown(KeyCode::ARROW_LEFT);
	rght = keys->keyDown(KeyCode::ARROW_RIGHT);
	up = keys->keyDown(KeyCode::ARROW_UP);
	down = keys->keyDown(KeyCode::ARROW_DOWN);

	// Debug mouse
	/*if (_touchProc) {
		CULog("Touch: %f %f\n", _dtouch.x, _dtouch.y);
		_touchProc = false;
	}*/


#else
    // MOBILE CONTROLS
    Vec3 acc = Input::get<Accelerometer>()->getAcceleration();

    // Pitch allows us to treat the tablet like a steering wheel
    float pitch = atan2(-acc.x, sqrt(acc.y*acc.y + acc.z*acc.z));
    
    // Check if we turned left or right
    left |= (pitch > EVENT_ACCEL_THRESH);
    rght |= (pitch < -EVENT_ACCEL_THRESH);
    up   |= _keyUp;
#endif

    _resetPressed = _keyReset;
    _debugPressed = _keyDebug; //THIS MAY HAVE CAUSED THE PROBLEM
    _exitPressed  = _keyExit;
    
    // Directional controls
    _horizontal = 0.0f;
    if (rght) {
        _horizontal += 1.0f;
    }
    if (left) {
        _horizontal -= 1.0f;
    }

    _vertical = 0.0f;
    if (up) {
        _vertical += 1.0f;
    }
    if (down) {
        _vertical -= 1.0f;
    }


// If it does not support keyboard, we must reset "virtual" keyboard
#ifdef CU_TOUCH_SCREEN
    _keyDebug = false;
    _keyReset = false;
    _keyDebug = false;
#endif
}

/**
 * Clears any buffered inputs so that we may start fresh.
 */
void RocketInput::clear() {
    _resetPressed = false;
    _debugPressed = false;
    _exitPressed  = false;
	_pausePressed = false;
    
    _horizontal = 0.0f;
    _vertical   = 0.0f;
    
    _dtouch = Vec2::ZERO;
    _timestamp.mark();
}

// PBB: need to add mouse version?
#pragma mark -
#pragma mark Mouse Callbacks
/**
* Callback for the beginning of a mouse event
*
* @param t     The mouse information
* @param event The associated event
*/
void RocketInput::mouseBeganCB(const cugl::MouseEvent& event, bool focus) {
	// All touches correspond to key up
	_keyUp = true;

	// Flag we have processed a touch between update frames
	_touchProc = true;

	// Update the touch location for later gestures
	// NOTE 0,0 is in the top left.
	_timestamp = event.timestamp;
	_dtouch = event.position;
}

/**
* Callback for the end of a touch event
*
* @param t     The mouse information
* @param event The associated event
*/
void RocketInput::mouseEndedCB(const cugl::MouseEvent& event, bool focus) {
	// Gesture has ended.  Give it meaning.
	Vec2 diff = event.position - _dtouch;
	bool fast = (event.timestamp.ellapsedMillis(_timestamp) < EVENT_SWIPE_TIME);
	_keyReset = fast && diff.x < -EVENT_SWIPE_LENGTH;
	_keyExit = fast && diff.x > EVENT_SWIPE_LENGTH;
	_keyDebug = fast && diff.y > EVENT_SWIPE_LENGTH; //THIS MAY HAVE CAUSED THE PROBLEM
	_keyUp = false;
}



#pragma mark -
#pragma mark Touch Callbacks
/**
 * Callback for the beginning of a touch event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void RocketInput::touchBeganCB(const cugl::TouchEvent& event, bool focus) {
    // All touches correspond to key up
    _keyUp = true;

	// Flag we have processed a touch between update frames
	_touchProc = true;
     
    // Update the touch location for later gestures
    _timestamp = event.timestamp;
    _dtouch = event.position;
}
 
/**
 * Callback for the end of a touch event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void RocketInput::touchEndedCB(const cugl::TouchEvent& event, bool focus) {
    // Gesture has ended.  Give it meaning.
    Vec2 diff = event.position-_dtouch;
    bool fast = (event.timestamp.ellapsedMillis(_timestamp) < EVENT_SWIPE_TIME);
    _keyReset = fast && diff.x < -EVENT_SWIPE_LENGTH;
    _keyExit  = fast && diff.x > EVENT_SWIPE_LENGTH;
    _keyDebug = fast && diff.y > EVENT_SWIPE_LENGTH; //THIS MAY HAVE CAUSED THE PROBLEM
    _keyUp = false;
}
