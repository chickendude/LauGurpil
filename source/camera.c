#include "camera.h"
#include "race.h"
#include "racecar.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------

void update_camera(Camera *camera, Racecar *focused_car)
{
    int x = (focused_car->x >> 12) - SCREEN_WIDTH / 2;
    if (camera->x - x > 15) x = camera->x - 15;
    if (camera->x - x < -15) x = camera->x + 15;
    int y = (focused_car->y >> 12) - SCREEN_HEIGHT / 2;
    if (camera->y - y > 15) y = camera->y - 15;
    if (camera->y - y < -15) y = camera->y + 15;
    camera->x = x;
    camera->y = y;
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------