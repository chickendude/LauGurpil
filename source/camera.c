#include "camera.h"
#include "race.h"
#include "racecar.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------

// TODO: Pass in car pointer directly
void update_camera(Camera *camera, Racecar *focused_car)
{
    camera->x = (focused_car->x >> 12) - SCREEN_WIDTH / 2;
    camera->y = (focused_car->y >> 12) - SCREEN_HEIGHT / 2;
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------