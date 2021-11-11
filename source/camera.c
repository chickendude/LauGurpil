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
void update_camera(Race *race, unsigned int car_on_camera)
{
    Camera *cam = &race->camera;
    Racecar *car = &race->cars[car_on_camera];
    cam->x = (car->x >> 12) - SCREEN_WIDTH / 2;
    cam->y = (car->y >> 12) - SCREEN_HEIGHT / 2;
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------