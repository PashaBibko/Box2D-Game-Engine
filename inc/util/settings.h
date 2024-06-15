#pragma once

// All custom settings are defined here

// --------------------------------------------------------------------------------------------------------------------- //
// Changing these settings is supported by the engine																	 //
// --------------------------------------------------------------------------------------------------------------------- //

// Maximum gravity an object can have
// NOT A VELOCITY CAP ONLY A GRAVITY CAP
constexpr float MAX_GRAVITY = 100.0f;

// Gravity applied each frame
constexpr float GRAVITY_STRENGTH = 9.8f;

// Whether or not to allow the object to have a maximum velocity
constexpr bool MAX_VELOCITY_ENABLED = true;

// While true it will allow the object to have a higher velocity than the max velocity
// but will not be able to accelerate any further
constexpr bool MAX_VELOCITY_SOFT_CAP_ENABLED = false;

// Maximum x-velocity an object can have
constexpr float MAX_X_VELOCITY = 100.0f;

// Maximum y-velocity an object can have
constexpr float MAX_Y_VELOCITY = 100.0f;

// --------------------------------------------------------------------------------------------------------------------- //
// Modifying any of the settings below is not fully supported by the engine 											 //
// Editing these settings may cause the engine to not function propely or not at all 									 //
// --------------------------------------------------------------------------------------------------------------------- //

// Whether or not to allow multiple instances of the engine
// If set to false, the engine will only allow one instance to be created (recommended)
constexpr bool ALLOW_MULTIPLE_INSTANCES = false;