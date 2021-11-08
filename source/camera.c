#include "camera.h"
#include "race.h"
#include "racecar.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------

void update_camera(Race *race, int car_on_camera)
{
    Camera *cam = &race->camera;
    Racecar *car =
            car_on_camera == 0 ? race->car :
            &race->computer_cars[car_on_camera - 1];
    cam->x = (car->x >> 12) - SCREEN_WIDTH / 2;
    cam->y = (car->y >> 12) - SCREEN_HEIGHT / 2;
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------