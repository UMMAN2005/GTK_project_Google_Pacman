#include "app.h"

enum Direction nextDirection;
enum Direction requestedDirection;

unsigned int stayedPoints = 277;
unsigned int totalPoints = 0;
unsigned int currentLevel = 1;
unsigned int timeout;
const unsigned int maxLevel = 5;
clock_t startTime;
time_t activationTime;
guint labelUpdateTimeoutId = 0;
GtkCssProvider *cssProvider = NULL;
GtkWidget *window;
GtkWidget *grid;
GtkWidget *drawing_area;
GtkWidget *label;
time_t enterTimePinky;
time_t enterTimeInky;
time_t enterTimeClyde;
time_t enterTimeBlinky;
bool newPointCollectedBlinky;
bool newPointCollectedPinky;
bool newPointCollectedInky;
bool newPointCollectedClyde;
bool inBoxPinky = true;
bool inBoxInky = true;
bool inBoxClyde = true;
bool inBoxBlinky = true;
bool outBoxPinky = false;
bool outBoxInky = false;
bool outBoxClyde = false;
bool outBoxBlinky = false;
Square* square;
Ghost* ghost1;
Ghost* ghost2;
Ghost* ghost3;
Ghost* ghost4;
Timer* timer;
const GdkRGBA PACMAN_COLOR = {1.0, 1.0, 0.0, 1.0};
const GdkRGBA GHOST_COLOR_1 = {1.0, 0.0, 0.0, 1.0};
const GdkRGBA GHOST_COLOR_2 = {1.0, 184 / 255.0, 1.0, 1.0};
const GdkRGBA GHOST_COLOR_3 = {0.0, 1.0, 1.0, 1.0};
const GdkRGBA GHOST_COLOR_4 = {1.0, 184 / 255.0, 82 / 255.0, 1.0};
const GdkRGBA GHOST_COLOR_F = {25 / 255.0, 25 / 255.0, 166 / 255.0, 1.0};

void printTitle() {
    printf(COLOR_GREEN);
    printSlowly(" .----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .-----------------.");
    printSlowly("| .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |");
    printSlowly("| |   ______     | || |      __      | || |     ______   | || | ____    ____ | || |      __      | || | ____  _____  | |");
    printSlowly("| |  |_   __ \\   | || |     /  \\     | || |   .' ___  |  | || ||_   \\  /   _|| || |     /  \\     | || ||_   \\|_   _| | |");
    printSlowly("| |    | |__) |  | || |    / /\\ \\    | || |  / .'   \\_|  | || |  |   \\/   |  | || |    / /\\ \\    | || |  |   \\ | |   | |");
    printSlowly("| |    |  ___/   | || |   / ____ \\   | || |  | |         | || |  | |\\  /| |  | || |   / ____ \\   | || |  | |\\ \\| |   | |");
    printSlowly("| |   _| |_      | || | _/ /    \\ \\_ | || |  \\ `.___.'\\  | || | _| |_\\/_| |_ | || | _/ /    \\ \\_ | || | _| |_\\   |_  | |");
    printSlowly("| |  |_____|     | || ||____|  |____|| || |   `._____.'  | || ||_____||_____|| || ||____|  |____|| || ||_____|\\____| | |");
    printSlowly("| |              | || |              | || |              | || |              | || |              | || |              | |");
    printSlowly("| '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |");
    printSlowly(" '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------' ");
    printf(COLOR_RESET);
}

void printSlowly(const char *str) {
    while (*str) {
        putchar(*str);
        fflush(stdout);

        struct timespec delay;
        delay.tv_sec = 0;
        delay.tv_nsec = 1000000;

        nanosleep(&delay, NULL);

        str++;
    }
    putchar('\n');
    fflush(stdout);
}

gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    Square *square = (Square *)user_data;

    double originalX = square->x;
    double originalY = square->y;

    // Save the current direction in a temporary variable
    nextDirection = square->currentDirection;

    // Update the current direction based on the pressed key
    switch (event->keyval) {
        case GDK_KEY_Up:
        case GDK_KEY_w:
            nextDirection = UP;
            break;
        case GDK_KEY_Down: 
        case GDK_KEY_s:
            nextDirection = DOWN;
            break;
        case GDK_KEY_Left:
        case GDK_KEY_a:
            nextDirection = LEFT;
            break;
        case GDK_KEY_Right:
        case GDK_KEY_d:
            nextDirection = RIGHT;
            break;
        default:
            break;
    }
    
    // Comment out for more basic game mode!
    /**/requestedDirection = nextDirection;/**/
    // Comment out for more basic game mode!

    // Check if turning is possible based on the special collision function
    if (!special_check_collision(square, walls, NUM_WALLS, nextDirection)) {
        // If turning is possible, update the current direction
        square->currentDirection = nextDirection;

        // Move the square based on the updated current direction with the defined speed
        switch (square->currentDirection) {
            case UP:
                if (square->y - SPEED >= 0) {
                    square->y -= SPEED;
                }
                break;
            case DOWN:
                if (square->y + SQUARE_SIZE + SPEED <= WINDOW_HEIGHT) {
                    square->y += SPEED;
                }
                break;
            case LEFT:
                if (square->x - SPEED >= 0) {
                    square->x -= SPEED;
                }
                break;
            case RIGHT:
                if (square->x + SQUARE_SIZE + SPEED <= WINDOW_WIDTH) {
                    square->x += SPEED;
                }
                break;
            default:
                break;
        }
    }  else {
    // Check if turning is possible with tolerance
    enum Direction upDirection = UP;
    enum Direction downDirection = DOWN;
    enum Direction leftDirection = LEFT;
    enum Direction rightDirection = RIGHT;

    if (!special_check_collision(square, walls, NUM_WALLS, upDirection) &&
        distanceToWall(square, upDirection) < TURNING_TOLERANCE) {
        square->currentDirection = upDirection;
    } else if (!special_check_collision(square, walls, NUM_WALLS, downDirection) &&
               distanceToWall(square, downDirection) < TURNING_TOLERANCE) {
        square->currentDirection = downDirection;
    } else if (!special_check_collision(square, walls, NUM_WALLS, leftDirection) &&
               distanceToWall(square, leftDirection) < TURNING_TOLERANCE) {
        square->currentDirection = leftDirection;
    } else if (!special_check_collision(square, walls, NUM_WALLS, rightDirection) &&
               distanceToWall(square, rightDirection) < TURNING_TOLERANCE) {
        square->currentDirection = rightDirection;
    }
}

    // Check for collisions with walls
    if (check_collision((Entity*)square, walls, NUM_WALLS)) {
        // If there's a collision, revert the position
        square->x = originalX;
        square->y = originalY;
    }

    if (check_collision_with_points(square, points, NUM_POINTS)) {
        // Handle point collection (e.g., increase score)
    }

    if (check_portal((Entity*)square) == 1) {
        square->x = 1160;
        square->y = originalY;
    } else if (check_portal((Entity*)square) == 2) {
        square->x = 0;
        square->y = originalY;
    } else if (check_portal((Entity*)square) == 3) {
        square->x = originalX;
        square->y = originalY;
    }

    // Redraw the window
    gtk_widget_queue_draw(widget);

    return G_SOURCE_CONTINUE;
}


void activate(GtkApplication *app, gpointer user_data) {
    clearScreen();
    printTitle();
    char info[256];
    sprintf(info, "The source code can be found at:\nhttps://github.com/UMMAN2005/GTK_project_Google_Pacman\
    \n\nIf you want to change the source code, please send a pull request.");
    showInfoDialog(info);

    activationTime;
    time(&activationTime);
    timeout = 0;

    srand(time(NULL));

    // Create the main window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Google Pacman");
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_style_context_add_class(gtk_widget_get_style_context(window), "window");
    setColor(window, NULL);

    // Create the grid and the label

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    label = gtk_label_new(g_strdup_printf("🪙: %d\t⚡: %d\t⏳: %.0lf\t🛡️: %d", 
    totalPoints, currentLevel, getElapsedTime(), timeout));
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_style_context_add_class(gtk_widget_get_style_context(label), "label");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    // Create the drawing area
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_grid_attach(GTK_GRID(grid), drawing_area, 0, 1, 1, 1);

    g_timeout_add_seconds(1, updateLabelPeriodically, label);

    // Set up the square
    square = g_malloc(sizeof(Square));
    square->x = 740;
    square->y = 290;
    strncpy(square->name, "Pacman", sizeof(square->name));
    strncpy(square->nickName, "", sizeof(square->nickName));
    square->currentDirection = LEFT;

    // Set up the ghosts
    ghost1 = g_malloc(sizeof(Ghost));
    ghost2 = g_malloc(sizeof(Ghost));
    ghost3 = g_malloc(sizeof(Ghost));
    ghost4 = g_malloc(sizeof(Ghost));
    ghost1->x = 740;
    ghost1->y = 70;
    ghost1->scatterX = 1110;
    ghost1->scatterY = 60;
    ghost1->frightenedModeTimeout = 10 - currentLevel;
    ghost1->ghostSpeed = 5.0;
    ghost1->frightenedModeActive = false;
    ghost1->frightenedModeStart = activationTime + 1000;
    strncpy(ghost1->name, "Shadow", sizeof(ghost1->name));
    strncpy(ghost1->nickName, "Blinky", sizeof(ghost1->nickName));
    ghost1->currentDirection = UP;

    ghost2->x = 740;
    ghost2->y = 130;
    ghost2->ghostSpeed = 5.0;
    ghost2->frightenedModeTimeout = 10 - currentLevel;
    ghost2->scatterX = 70;
    ghost2->scatterY = 60;
    ghost2->frightenedModeActive = false;
    ghost2->frightenedModeStart = activationTime + 1000;
    strncpy(ghost2->name, "Speedy", sizeof(ghost2->name));
    strncpy(ghost2->nickName, "Pinky", sizeof(ghost2->nickName));
    ghost2->currentDirection = DOWN;

    ghost3->x = 700;
    ghost3->y = 130;
    ghost3->ghostSpeed = 5.0;
    ghost3->frightenedModeTimeout = 10 - currentLevel;
    ghost3->scatterX = 1110;
    ghost3->scatterY = 280;
    ghost3->frightenedModeActive = false;
    ghost3->frightenedModeStart = activationTime + 1000;
    strncpy(ghost3->name, "Bashful", sizeof(ghost3->name));
    strncpy(ghost3->nickName, "Inky", sizeof(ghost3->nickName));
    ghost3->currentDirection = LEFT;

    ghost4->x = 780;
    ghost4->y = 130;
    ghost4->ghostSpeed = 5.0;
    ghost4->frightenedModeTimeout = 10 - currentLevel;
    ghost4->scatterX = 80;
    ghost4->scatterY = 280;
    ghost4->frightenedModeActive = false;
    ghost4->frightenedModeStart = activationTime + 1000;
    strncpy(ghost4->name, "Pokey", sizeof(ghost4->name));
    strncpy(ghost4->nickName, "Clyde", sizeof(ghost4->nickName));
    ghost4->currentDirection = RIGHT;
    
    // Connect signals
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), square);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), ghost1);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), ghost2);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), ghost3);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), ghost4);
    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), square);

    g_timeout_add(SELF_SPEED, key_press_timeout_callback, square);

    startTime = clock();
    
    gtk_widget_show_all(window);
}

bool check_collision_with_walls(double x, double y, double width, double height) {
    for (int i = 0; i < NUM_WALLS; i++) {
        if (x < walls[i].x + walls[i].width &&
            x + width > walls[i].x &&
            y < walls[i].y + walls[i].height &&
            y + height > walls[i].y) {
            return true; // Collision detected
        }
    }
    return false; // No collision
}

void moveBlinky(Ghost *blinky, Square *pacman) {
    if (getElapsedTime(timer) < 0) {
        return;
    }

    double originalX = blinky->x;
    double originalY = blinky->y;

    static bool startedBlinky = true;

    if (startedBlinky) {
        inBoxBlinky = false;
        startedBlinky = false;
        blinky->x = 740;
        blinky->y = 70;
    }

    if (inBoxBlinky && !startedBlinky) {
        time_t currentTime;
        time(&currentTime);
        if (difftime(currentTime, enterTimeBlinky) == 6 - currentLevel) {
            blinky->x = 740;
            blinky->y = 70;
            inBoxBlinky = false;
            startedBlinky = false;
            blinky->frightenedModeActive = false;
            blinky->frightenedModeStart += 1000;
            outBoxBlinky = true;
        }
    }

    time_t currentTime;
    time(&currentTime);
    double timeDifference = difftime(currentTime, blinky->frightenedModeStart);

    double targetX;
    double targetY;

    // Set target coordinates based on mode
    if ((timeDifference <= blinky->frightenedModeTimeout && timeDifference >= 0)
    || ((blinky->x == 230 && blinky->y == 150) || (blinky->x == 990 && blinky->y == 170))) {
        targetX = blinky->scatterX;
        targetY = blinky->scatterY;
        if (newPointCollectedBlinky || ((blinky->x == 230 && blinky->y == 150) 
        || (blinky->x == 990 && blinky->y == 170))) {
        blinky->currentDirection = getOppositeDirection(blinky->currentDirection);
        newPointCollectedBlinky = false;
        }
    } else {
        // Calculate the direction towards Pacman
        targetX = pacman->x;
        targetY = pacman->y;
    }

    // Normalize the direction vector
    double directionX = targetX - blinky->x;
    double directionY = targetY - blinky->y;

    // Normalize the direction vector
    double distance = calculateDistance(0, 0, directionX, directionY);
    if (distance != 0) {
        directionX /= distance;
        directionY /= distance;
    }

    enum Direction alternativeDirections[3]; // Exclude opposite direction
    double alternativeX[3];
    double alternativeY[3];
    int numDirections = 0;

    for (int i = 0; i < 4; i++) {
        if (i != getOppositeDirection(blinky->currentDirection)) {
            alternativeX[numDirections] = blinky->x;
            alternativeY[numDirections] = blinky->y;

            // Calculate the alternative direction
            switch (i) {
                case UP:
                    alternativeY[numDirections] -= blinky->ghostSpeed;
                    break;
                case RIGHT:
                    alternativeX[numDirections] += blinky->ghostSpeed;
                    break;
                case DOWN:
                    alternativeY[numDirections] += blinky->ghostSpeed;
                    break;
                case LEFT:
                    alternativeX[numDirections] -= blinky->ghostSpeed;
                    break;
            }

            alternativeDirections[numDirections] = i;
            numDirections++;
        }
    }

    // Find the most efficient direction
    int mostEfficientDirection = -1;
    double minDistance = DBL_MAX;

    for (int i = 0; i < numDirections; i++) {
        // Check for collisions with walls for the alternative direction
        if (!check_collision_with_walls(alternativeX[i], alternativeY[i], SQUARE_SIZE, SQUARE_SIZE)) {
            // Calculate distance to Pacman for the alternative position
            double distanceToTarget = calculateDistance(alternativeX[i], alternativeY[i], targetX, targetY);

            // Update the most efficient direction if the current alternative is closer to target
            if (distanceToTarget < minDistance) {
                minDistance = distanceToTarget;
                mostEfficientDirection = i;
            }
        }
    }

    // Move in the most efficient direction
    if (mostEfficientDirection != -1) {
        blinky->x = alternativeX[mostEfficientDirection];
        blinky->y = alternativeY[mostEfficientDirection];
        if (check_portal((Entity*)blinky) == 1) {
            blinky->x = 1160;
            blinky->y = originalY;
        } else if (check_portal((Entity*)blinky) == 2) {
            blinky->x = 0;
            blinky->y = originalY;
        } else if (check_portal((Entity*)blinky) == 3) {
            blinky->x = originalX;
            blinky->y = 70;
        }
        blinky->currentDirection = alternativeDirections[mostEfficientDirection];
    }
}

void movePinky(Ghost *pinky, Square *pacman) {
    // Check if the ghost should start moving
    if (getElapsedTime(timer) < 5) {
        return; // Ghost should not move yet
    }

    double originalX = pinky->x;
    double originalY = pinky->y;

    static bool startedPinky = true;

    if (startedPinky) {
        inBoxPinky = false;
        startedPinky = false;
        pinky->x = 740;
        pinky->y = 70;
    }

    if (inBoxPinky && !startedPinky) {
        time_t currentTime;
        time(&currentTime);
        if (difftime(currentTime, enterTimePinky) == 6 - currentLevel) {
            pinky->x = 740;
            pinky->y = 70;
            inBoxPinky = false;
            startedPinky = false;
            pinky->frightenedModeActive = false;
            pinky->frightenedModeStart += 1000;
            outBoxPinky = true;
        }
    }


    time_t currentTime;
    time(&currentTime);
    double timeDifference = difftime(currentTime, pinky->frightenedModeStart);

    double targetX, targetY;

    // Set target coordinates based on mode
    if ((timeDifference <= pinky->frightenedModeTimeout && timeDifference >= 0)
    || ((pinky->x == 230 && pinky->y == 150) || (pinky->x == 990 && pinky->y == 170))) {
        targetX = pinky->scatterX;
        targetY = pinky->scatterY;
        if (newPointCollectedPinky || ((pinky->x == 230 && pinky->y == 150) 
        || (pinky->x == 990 && pinky->y == 170))) {
        pinky->currentDirection = getOppositeDirection(pinky->currentDirection);
        newPointCollectedPinky = false;
        }
    } else {
        // Calculate the direction towards Pacman
        targetX = pacman->x;
        targetY = pacman->y;
        switch (pacman->currentDirection) {
            case UP:
                targetY -= 80;
                break;
            case RIGHT:
                targetX += 80;
                break;
            case DOWN:
                targetY += 80;
                break;
            case LEFT:
                targetX -= 80;
                break;
        }
    }

    // Calculate the direction towards the target
    double directionX = targetX - pinky->x;
    double directionY = targetY - pinky->y;

    // Normalize the direction vector
    double distance = calculateDistance(0, 0, directionX, directionY);
    if (distance != 0) {
        directionX /= distance;
        directionY /= distance;
    }

    // Try alternative directions with priority: up, right, down, left
    enum Direction alternativeDirections[3]; // Exclude opposite direction
    double alternativeX[3];
    double alternativeY[3];
    int numDirections = 0;

    for (int i = 0; i < 4; i++) {
        if (i != getOppositeDirection(pinky->currentDirection)) {
            alternativeX[numDirections] = pinky->x;
            alternativeY[numDirections] = pinky->y;

            // Calculate the alternative direction
            switch (i) {
                case UP:
                    alternativeY[numDirections] -= pinky->ghostSpeed;
                    break;
                case RIGHT:
                    alternativeX[numDirections] += pinky->ghostSpeed;
                    break;
                case DOWN:
                    alternativeY[numDirections] += pinky->ghostSpeed;
                    break;
                case LEFT:
                    alternativeX[numDirections] -= pinky->ghostSpeed;
                    break;
            }

            alternativeDirections[numDirections] = i;
            numDirections++;
        }
    }

    // Find the most efficient direction
    int mostEfficientDirection = -1;
    double minDistance = DBL_MAX;

    for (int i = 0; i < numDirections; i++) {
        // Check for collisions with walls for the alternative direction
        if (!check_collision_with_walls(alternativeX[i], alternativeY[i], SQUARE_SIZE, SQUARE_SIZE)) {
            // Calculate distance to Pacman for the alternative position
            double distanceToPacman = calculateDistance(alternativeX[i], alternativeY[i], targetX, targetY);

            // Update the most efficient direction if the current alternative is closer to Pacman
            if (distanceToPacman < minDistance) {
                minDistance = distanceToPacman;
                mostEfficientDirection = i;
            }
        }
    }

    // Move in the most efficient direction
    if (mostEfficientDirection != -1) {
        pinky->x = alternativeX[mostEfficientDirection];
        pinky->y = alternativeY[mostEfficientDirection];
        if (check_portal((Entity*)pinky) == 1) {
            pinky->x = 1160;
            pinky->y = originalY;
        } else if (check_portal((Entity*)pinky) == 2) {
            pinky->x = 0;
            pinky->y = originalY;
        } else if (check_portal((Entity*)pinky) == 3) {
            pinky->x = originalX;
            pinky->y = 70;
        }
        pinky->currentDirection = alternativeDirections[mostEfficientDirection];
    }
}

void moveInky(Ghost *inky, Ghost *blinky, Square *pacman) {
    // Check if the ghost should start moving
    if (getElapsedTime(timer) < 13) {
        return; // Ghost should not move yet
    }

    double originalX = inky->x;
    double originalY = inky->y;

    static bool startedInky = true;

    if (startedInky) {
        inBoxInky = false;
        startedInky = false;
        inky->x = 740;
        inky->y = 70;
    }
    
    if (inBoxInky && !startedInky) {
        time_t currentTime;
        time(&currentTime);
        if (difftime(currentTime, enterTimeInky) == 6 - currentLevel) {
            inky->x = 740;
            inky->y = 70;
            inBoxInky = false;
            startedInky = false;
            inky->frightenedModeActive = false;
            inky->frightenedModeStart += 1000;
            outBoxInky = true;
        }
    }

    time_t currentTime;
    time(&currentTime);
    double timeDifference = difftime(currentTime, inky->frightenedModeStart);

    double targetX, targetY;

    // Set target coordinates based on mode
    if ((timeDifference <= inky->frightenedModeTimeout && timeDifference >= 0)
    || ((inky->x == 230 && inky->y == 150) || (inky->x == 990 && inky->y == 170))) {
        targetX = inky->scatterX;
        targetY = inky->scatterY;
        if (newPointCollectedInky || ((inky->x == 230 && inky->y == 150) 
        || (inky->x == 990 && inky->y == 170))) {
        inky->currentDirection = getOppositeDirection(inky->currentDirection);
        newPointCollectedInky = false;
        }
    } else {
        // Calculate the direction towards Pacman
        targetX = pacman->x;
        targetY = pacman->y;

        switch (pacman->currentDirection) {
            case UP:
                targetY -= 40;
                break;
            case RIGHT:
                targetX += 40;
                break;
            case DOWN:
                targetY += 40;
                break;
            case LEFT:
                targetX -= 40;
                break;
        }    

        // Calculate the vector from Blinky to the target
        double vectorX = targetX - blinky->x;
        double vectorY = targetY - blinky->y;

        // Rotate the vector 180 degrees
        targetX = targetX + vectorX;
        targetY = targetY + vectorY;
    }
    // Calculate the direction towards the target
    double directionX = targetX - inky->x;
    double directionY = targetY - inky->y;

    // Normalize the direction vector
    double distance = calculateDistance(0, 0, directionX, directionY);
    if (distance != 0) {
        directionX /= distance;
        directionY /= distance;
    }

    // Try alternative directions with priority: up, right, down, left
    enum Direction alternativeDirections[3]; // Exclude opposite direction
    double alternativeX[3];
    double alternativeY[3];
    int numDirections = 0;

    for (int i = 0; i < 4; i++) {
        if (i != getOppositeDirection(inky->currentDirection)) {
            alternativeX[numDirections] = inky->x;
            alternativeY[numDirections] = inky->y;

            // Calculate the alternative direction
            switch (i) {
                case UP:
                    alternativeY[numDirections] -= inky->ghostSpeed;
                    break;
                case RIGHT:
                    alternativeX[numDirections] += inky->ghostSpeed;
                    break;
                case DOWN:
                    alternativeY[numDirections] += inky->ghostSpeed;
                    break;
                case LEFT:
                    alternativeX[numDirections] -= inky->ghostSpeed;
                    break;
            }

            alternativeDirections[numDirections] = i;
            numDirections++;
        }
    }

    // Find the most efficient direction
    int mostEfficientDirection = -1;
    double minDistance = DBL_MAX;

    for (int i = 0; i < numDirections; i++) {
        // Check for collisions with walls for the alternative direction
        if (!check_collision_with_walls(alternativeX[i], alternativeY[i], SQUARE_SIZE, SQUARE_SIZE)) {
            // Calculate distance to target for the alternative position
            double distanceToTarget = calculateDistance(alternativeX[i], alternativeY[i], targetX, targetY);

            // Update the most efficient direction if the current alternative is closer to Blinky
            if (distanceToTarget < minDistance) {
                minDistance = distanceToTarget;
                mostEfficientDirection = i;
            }
        }
    }

    // Move in the most efficient direction
    if (mostEfficientDirection != -1) {
        inky->x = alternativeX[mostEfficientDirection];
        inky->y = alternativeY[mostEfficientDirection];
        if (check_portal((Entity*)inky) == 1) {
            inky->x = 1160;
            inky->y = originalY;
        } else if (check_portal((Entity*)inky) == 2) {
            inky->x = 0;
            inky->y = originalY;
        } else if (check_portal((Entity*)inky) == 3) {
            inky->x = originalX;
            inky->y = 70;
        }
        inky->currentDirection = alternativeDirections[mostEfficientDirection];
    }
}

void moveClyde(Ghost *clyde, Square *pacman) {
    // Check if the ghost should start moving
    if (getElapsedTime(timer) < 24) {
        return; // Ghost should not move yet
    }

    double originalX = clyde->x;
    double originalY = clyde->y;

    static bool startedClyde = true;

    if (startedClyde) {
        inBoxClyde = false;
        startedClyde = false;
        clyde->x = 740;
        clyde->y = 70;
    }

    if (inBoxClyde && !startedClyde) {
        time_t currentTime;
        time(&currentTime);
        if (difftime(currentTime, enterTimeClyde) == 6 - currentLevel) {
            clyde->x = 740;
            clyde->y = 70;
            inBoxClyde = false;
            startedClyde = false;
            clyde->frightenedModeActive = false;
            clyde->frightenedModeStart += 1000;
            outBoxClyde = true;
        }
    }


    time_t currentTime;
    time(&currentTime);
    double timeDifference = difftime(currentTime, clyde->frightenedModeStart);

    double targetX, targetY;

    // Set target coordinates based on mode
    if ((timeDifference <= clyde->frightenedModeTimeout && timeDifference >= 0)
    || ((clyde->x == 230 && clyde->y == 150) || (clyde->x == 990 && clyde->y == 170))) {
        targetX = clyde->scatterX;
        targetY = clyde->scatterY;
        if (newPointCollectedClyde || ((clyde->x == 230 && clyde->y == 150) 
        || (clyde->x == 990 && clyde->y == 170))) {
        clyde->currentDirection = getOppositeDirection(clyde->currentDirection);
        newPointCollectedClyde = false;
        }
    } else {
        // Calculate the distance to Pacman
        double distanceToPacman = calculateDistance(clyde->x, clyde->y, pacman->x, pacman->y);


        if (distanceToPacman >= 160) {
            // If Clyde is far from Pacman, target is same as Blinky
            targetX = pacman->x;
            targetY = pacman->y;
        } else {
            targetX = rand() % WINDOW_WIDTH;
            targetY = rand() % WINDOW_HEIGHT;
        }
    }

    // Calculate the direction towards the target
    double directionX = targetX - clyde->x;
    double directionY = targetY - clyde->y;

    // Normalize the direction vector
    double distance = calculateDistance(0, 0, directionX, directionY);
    if (distance != 0) {
        directionX /= distance;
        directionY /= distance;
    }

    // Try alternative directions with priority: up, right, down, left
    enum Direction alternativeDirections[3]; // Exclude opposite direction
    double alternativeX[3];
    double alternativeY[3];
    int numDirections = 0;

    for (int i = 0; i < 4; i++) {
        if (i != getOppositeDirection(clyde->currentDirection)) {
            alternativeX[numDirections] = clyde->x;
            alternativeY[numDirections] = clyde->y;

            // Calculate the alternative direction
            switch (i) {
                case UP:
                    alternativeY[numDirections] -= clyde->ghostSpeed;
                    break;
                case RIGHT:
                    alternativeX[numDirections] += clyde->ghostSpeed;
                    break;
                case DOWN:
                    alternativeY[numDirections] += clyde->ghostSpeed;
                    break;
                case LEFT:
                    alternativeX[numDirections] -= clyde->ghostSpeed;
                    break;
            }

            alternativeDirections[numDirections] = i;
            numDirections++;
        }
    }


    // Find the most efficient direction
    int mostEfficientDirection = -1;
    double minDistance = DBL_MAX;

    for (int i = 0; i < numDirections; i++) {
        // Check for collisions with walls for the alternative direction
        if (!check_collision_with_walls(alternativeX[i], alternativeY[i], SQUARE_SIZE, SQUARE_SIZE)) {
            // Calculate distance to target for the alternative position
            double distanceToTarget = calculateDistance(alternativeX[i], alternativeY[i], targetX, targetY);

            // Update the most efficient direction if the current alternative is closer to the target
            if (distanceToTarget < minDistance) {
                minDistance = distanceToTarget;
                mostEfficientDirection = i;
            }
        }
    }

    // Move in the most efficient direction
    if (mostEfficientDirection != -1) {
        clyde->x = alternativeX[mostEfficientDirection];
        clyde->y = alternativeY[mostEfficientDirection];
        if (check_portal((Entity*)clyde) == 1) {
            clyde->x = 1160;
            clyde->y = originalY;
        } else if (check_portal((Entity*)clyde) == 2) {
            clyde->x = 0;
            clyde->y = originalY;
        } else if (check_portal((Entity*)clyde) == 3) {
            clyde->x = originalX;
            clyde->y = 70;
        }
        clyde->currentDirection = alternativeDirections[mostEfficientDirection];
    }
}

void draw_walls(cairo_t *cr, Wall *walls, int num_walls) {
    for (int i = 0; i < num_walls; i++) {
        // Set the fill color for the walls/
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_rectangle(cr, walls[i].x, walls[i].y, walls[i].width, walls[i].height);
        cairo_fill(cr);

        // Set the border color for the walls
        cairo_set_source_rgb(cr, walls[i].color[0], walls[i].color[1], walls[i].color[2]);
        cairo_rectangle(cr, walls[i].x, walls[i].y, walls[i].width, walls[i].height);
        cairo_set_line_width(cr, BORDER);
        cairo_stroke(cr);
    }
}

void addFruit(cairo_t *cr) {
    cairo_rectangle(cr, points[0].x, points[0].y, points[0].width, points[0].height);
    cairo_set_source_rgb(cr, 0, 1, 0);
    cairo_fill(cr);
}

void draw_points(cairo_t *cr, Point *points, int num_points) {
    for (int i = 0; i < num_points; i++) {
        if (!points[i].isCollected) {
            cairo_rectangle(cr, points[i].x, points[i].y, points[i].width, points[i].height);
            cairo_set_source_rgb(cr, 222/255.0, 161/255.0, 133/255.0);
            if (points[i].width == 20)
            cairo_set_source_rgb(cr, 0, 1, 0);
            cairo_fill(cr);
        }
    }
}

void draw_figure(cairo_t *cr, void *figure) {

    time_t currentTime;
    time(&currentTime);

    if (currentTime - activationTime >= 30 && currentTime - activationTime < 31) {
        addFruit(cr);
        points[0].isCollected = false;
    } else if (currentTime - activationTime >= 45 && currentTime - activationTime < 46) {
        points[0].isCollected = true;
    }


    double timeDifferenceBlinky = difftime(currentTime, ghost1->frightenedModeStart);
    double timeDifferencePinky = difftime(currentTime, ghost2->frightenedModeStart);
    double timeDifferenceInky = difftime(currentTime, ghost3->frightenedModeStart);
    double timeDifferenceClyde = difftime(currentTime, ghost4->frightenedModeStart);

    if (figure == NULL) {
        return;
    }

    if (figure == ghost1) {
        cairo_set_source_rgb(cr, GHOST_COLOR_1.red, GHOST_COLOR_1.green, GHOST_COLOR_1.blue);
        if (timeDifferenceBlinky <= ghost1->frightenedModeTimeout && timeDifferenceBlinky >= 0) {
            cairo_set_source_rgb(cr, GHOST_COLOR_F.red, GHOST_COLOR_F.green, GHOST_COLOR_F.blue);
        }

    } else if (figure == ghost2) {
        cairo_set_source_rgb(cr, GHOST_COLOR_2.red, GHOST_COLOR_2.green, GHOST_COLOR_2.blue);
        if (timeDifferencePinky <= ghost2->frightenedModeTimeout && timeDifferencePinky >= 0) {
            cairo_set_source_rgb(cr, GHOST_COLOR_F.red, GHOST_COLOR_F.green, GHOST_COLOR_F.blue);
        }

    } else if (figure == ghost3) {
        cairo_set_source_rgb(cr, GHOST_COLOR_3.red, GHOST_COLOR_3.green, GHOST_COLOR_3.blue);
        if (timeDifferenceInky <= ghost3->frightenedModeTimeout && timeDifferenceInky >= 0) {
            cairo_set_source_rgb(cr, GHOST_COLOR_F.red, GHOST_COLOR_F.green, GHOST_COLOR_F.blue);
        }

    } else if (figure == ghost4) {
        cairo_set_source_rgb(cr, GHOST_COLOR_4.red, GHOST_COLOR_4.green, GHOST_COLOR_4.blue);
        if (timeDifferenceClyde <= ghost4->frightenedModeTimeout && timeDifferenceClyde >= 0) {
            cairo_set_source_rgb(cr, GHOST_COLOR_F.red, GHOST_COLOR_F.green, GHOST_COLOR_F.blue);
        }

    } else if (figure == square) {
        cairo_set_source_rgb(cr, PACMAN_COLOR.red, PACMAN_COLOR.green, PACMAN_COLOR.blue);
    }

    Square *f = (Square *)figure;
    cairo_rectangle(cr, f->x, f->y, SQUARE_SIZE, SQUARE_SIZE);
    cairo_fill(cr);
}

gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    Square *square = (Square *)user_data;

    // Draw walls
    draw_walls(cr, walls, NUM_WALLS);

    // Draw points
    draw_points(cr, points, NUM_POINTS);

    // Draw the square
    draw_figure(cr, square);
    draw_figure(cr, ghost1);
    draw_figure(cr, ghost2);
    draw_figure(cr, ghost3);
    draw_figure(cr, ghost4);

    return FALSE;
}

bool check_collision(Entity *entity, Wall *walls, int num_walls) {
    for (int i = 0; i < num_walls; i++) {
        if (entity->x < walls[i].x + walls[i].width &&
            entity->x + SQUARE_SIZE > walls[i].x &&
            entity->y < walls[i].y + walls[i].height &&
            entity->y + SQUARE_SIZE > walls[i].y) {
            return true;
        }
    }
    return false;
}

bool special_check_collision(Square *square, Wall *walls, int num_walls, enum Direction direction) {
    // Calculate the next position based on the commanded direction and half the speed
    double nextX = square->x;
    double nextY = square->y;

    switch (direction) {
        case UP:
            nextY -= SPEED / SPEED;
            break;
        case DOWN:
            nextY += (SQUARE_SIZE + SPEED) / SPEED;
            break;
        case LEFT:
            nextX -= SPEED / SPEED;
            break;
        case RIGHT:
            nextX += (SQUARE_SIZE + SPEED) / SPEED;
            break;
        default:
            break;
    }

    // Check for collisions with walls at the calculated next position
    for (int i = 0; i < num_walls; i++) {
        if (nextX < walls[i].x + walls[i].width &&
            nextX + SQUARE_SIZE > walls[i].x &&
            nextY < walls[i].y + walls[i].height &&
            nextY + SQUARE_SIZE > walls[i].y) {
            return true; // Collision detected
        }
    }

    return false; // No collision
}

bool check_collision_with_points(Square *square, Point *points, int num_points) {
    for (int i = 0; i < num_points; i++) {
        if (!points[i].isCollected &&
            square->x < points[i].x + points[i].width &&
            square->x + SQUARE_SIZE > points[i].x &&
            square->y < points[i].y + points[i].height &&
            square->y + SQUARE_SIZE > points[i].y) {
            // If there's a collision with an uncollected point, collect it
            if (points[i].height == 18) {
                totalPoints += 50;
                timeout = 10 - currentLevel;
                gtk_label_set_text(GTK_LABEL(label), g_strdup_printf("🪙: %d\t⚡: %d\t⏳: %.0lf\t🛡️: %d", 
                totalPoints, currentLevel, getElapsedTime(), timeout));
                time(&(ghost1->frightenedModeStart));
                time(&(ghost2->frightenedModeStart));
                time(&(ghost3->frightenedModeStart));
                time(&(ghost4->frightenedModeStart));
                ghost1->frightenedModeActive = true;
                ghost2->frightenedModeActive = true;
                ghost3->frightenedModeActive = true;
                ghost4->frightenedModeActive = true;
                newPointCollectedBlinky = true;
                newPointCollectedPinky = true;
                newPointCollectedInky = true;
                newPointCollectedClyde = true;
            } else if (points[i].height == 6) {
                totalPoints += 10;
            } else if (points[i].height == 20) {
                totalPoints += currentLevel * 100;
            }
            points[i].isCollected = true;
            stayedPoints--;
            if (stayedPoints == 0) {
                char info[100];
                sprintf(info, "Congrats you won!\nScore: %d", totalPoints);
                showInfoDialog(info);
                quitApplication();
                exit(0);
            }
        }
    }
    return false;
}

bool checkGhostCollision(Ghost *ghost, Square *pacman) {
    double distance = calculateDistance(ghost->x, ghost->y, pacman->x, pacman->y);
    return distance < SQUARE_SIZE;
}

void check_collision_with_ghosts(Ghost* ghosts[4], Square* pacman) {
    static time_t lastCollisionTime = 0;
    time_t currentTime;
    time_t timeNow;
    time(&timeNow);

    for (int i = 0; i < 4; i++) {
        if (((timeNow - ghosts[i]->frightenedModeStart <= ghosts[i]->frightenedModeTimeout)
        && timeNow - ghosts[i]->frightenedModeStart >= 0 && ghosts[i]->frightenedModeActive)
        && checkGhostCollision(ghosts[i], pacman)) {
            // Handle collision when frightened mode is active for the current ghost
            ghostEaten(ghosts[i]);

            // Calculate points based on the time difference
            time(&currentTime);
            double timeDifference = difftime(currentTime, lastCollisionTime);

            static int points = 200;

            // Double the points if Pacman eats another ghost in less than or equal to 3 seconds
            if (timeDifference <= 3) {
                points *= 2;
            } else {
                points = 200;
            }

            // Update the last collision time
            lastCollisionTime = currentTime;

            // Add points to Pacman's score
            totalPoints += points;

        } else if (!((timeNow - ghosts[i]->frightenedModeStart <= ghosts[i]->frightenedModeTimeout)
            && timeNow - ghosts[i]->frightenedModeStart >= 0 && ghosts[i]->frightenedModeActive)
            && checkGhostCollision(ghosts[i], pacman)) {
            // Handle collision when frightened mode is not active for the current ghost
            showInfoDialog("The ghost caught you!");
            quitApplication();
            exit(0);
            //system("C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\Google_Pacman\\pacman.exe");
            return;
        }
    }
}

void changeGhostColor(cairo_t *cr, Ghost *figure) {
    if (figure == NULL) {
        return;
    }

    if (figure == ghost1) {
        cairo_set_source_rgb(cr, GHOST_COLOR_1.red, GHOST_COLOR_1.green, GHOST_COLOR_1.blue);

    } else if (figure == ghost2) {
        cairo_set_source_rgb(cr, GHOST_COLOR_2.red, GHOST_COLOR_2.green, GHOST_COLOR_2.blue);

    } else if (figure == ghost3) {
        cairo_set_source_rgb(cr, GHOST_COLOR_3.red, GHOST_COLOR_3.green, GHOST_COLOR_3.blue);

    } else if (figure == ghost4) {
        cairo_set_source_rgb(cr, GHOST_COLOR_4.red, GHOST_COLOR_4.green, GHOST_COLOR_4.blue);
    }
}

void ghostEaten(Ghost* ghost) {
    if (ghost == NULL) {
        return;
    } else if (ghost == ghost1) {
        ghost1 ->x = 740;
        ghost1 ->y = 130;
        inBoxBlinky = true;
        outBoxBlinky = false;
        ghost1->frightenedModeActive = false;
        ghost1->frightenedModeStart += 1000;
        time(&enterTimeBlinky);
        g_signal_connect(drawing_area, "draw", G_CALLBACK(changeGhostColor), ghost1);
    } else if (ghost == ghost2) {
        ghost2 ->x = 740;
        ghost2 ->y = 130;
        ghost2->frightenedModeActive = false;
        ghost2->frightenedModeStart += 1000;
        inBoxPinky = true;
        outBoxPinky = false;
        time(&enterTimePinky);
        g_signal_connect(drawing_area, "draw", G_CALLBACK(changeGhostColor), ghost2);
    } else if (ghost == ghost3) {
        ghost3 ->x = 700;
        ghost3 ->y = 130;
        ghost3->frightenedModeActive = false;
        ghost3->frightenedModeStart += 1000;
        inBoxInky = true;
        outBoxInky = false;
        time(&enterTimeInky);
        g_signal_connect(drawing_area, "draw", G_CALLBACK(changeGhostColor), ghost3);
    } else if (ghost == ghost4) {
        ghost4 ->x = 780;
        ghost4 ->y = 130;
        ghost4->frightenedModeActive = false;
        ghost4->frightenedModeStart += 1000;
        inBoxClyde = true;
        outBoxClyde = false;
        time(&enterTimeClyde);
        g_signal_connect(drawing_area, "draw", G_CALLBACK(changeGhostColor), ghost4);
    }
}

int check_portal(Entity *entity) {
    if ((entity->x == 0) && (entity->y >= 150 && entity->y <= 190))
    return 1;
    else if ((entity->x == 1160) && (entity->y >= 150 && entity->y <= 190))
    return 2;
}

double distanceToWall(Square *square, enum Direction direction) {
    double distance = DBL_MAX;

    switch (direction) {
        case UP:
            for (int i = 0; i < NUM_WALLS; i++) {
                if (square->x < walls[i].x + walls[i].width &&
                    square->x + SQUARE_SIZE > walls[i].x &&
                    square->y - distance > walls[i].y + walls[i].height &&
                    square->y - distance < walls[i].y) {
                    distance = square->y - walls[i].y - walls[i].height;
                }
            }
            break;
        case DOWN:
            for (int i = 0; i < NUM_WALLS; i++) {
                if (square->x < walls[i].x + walls[i].width &&
                    square->x + SQUARE_SIZE > walls[i].x &&
                    square->y + SQUARE_SIZE + distance > walls[i].y &&
                    square->y + SQUARE_SIZE + distance < walls[i].y + walls[i].height) {
                    distance = walls[i].y - (square->y + SQUARE_SIZE);
                }
            }
            break;
        case LEFT:
            for (int i = 0; i < NUM_WALLS; i++) {
                if (square->y < walls[i].y + walls[i].height &&
                    square->y + SQUARE_SIZE > walls[i].y &&
                    square->x - distance > walls[i].x + walls[i].width &&
                    square->x - distance < walls[i].x) {
                    distance = square->x - walls[i].x - walls[i].width;
                }
            }
            break;
        case RIGHT:
            for (int i = 0; i < NUM_WALLS; i++) {
                if (square->y < walls[i].y + walls[i].height &&
                    square->y + SQUARE_SIZE > walls[i].y &&
                    square->x + SQUARE_SIZE + distance > walls[i].x &&
                    square->x + SQUARE_SIZE + distance < walls[i].x + walls[i].width) {
                    distance = walls[i].x - (square->x + SQUARE_SIZE);
                }
            }
            break;
        default:
            break;
    }

    return distance;
}

double getElapsedTime() {
    clock_t currentTime = clock();
    double elapsedTime = (double)(currentTime - startTime) / CLOCKS_PER_SEC;
    return elapsedTime;
}

void updateLabelCallback(GtkWidget *label) {
    gtk_label_set_text(GTK_LABEL(label), g_strdup_printf("🪙: %d\t⚡: %d\t⏳: %.0lf\t🛡️: %d", 
    totalPoints, currentLevel, getElapsedTime(), timeout));

    if (timeout > 0)
    timeout--;
}

gboolean updateLabelPeriodically(gpointer user_data) {
    GtkWidget* label = GTK_WIDGET(user_data);
    updateLabelCallback(label);
    return G_SOURCE_CONTINUE;
}

gboolean key_press_timeout_callback(gpointer user_data) {
    Square* square = (Square*)user_data;

    time_t currentTime;
    time(&currentTime);

    // Update Pacman's direction and position
    if (requestedDirection == square->currentDirection) {
        square->currentDirection = requestedDirection;
        requestedDirection = STAY;
    } else {
        if (!special_check_collision(square, walls, NUM_WALLS, requestedDirection)) {
            square->currentDirection = requestedDirection;
            requestedDirection = STAY;
        }
    }

    GdkEventKey dummy_event;
    on_key_press(window, &dummy_event, square);

    // Move Blinky independently/
    moveBlinky(ghost1, square);
    movePinky(ghost2, square);
    moveInky(ghost3, ghost1, square);
    moveClyde(ghost4, square);

    Ghost* ghosts[4] = {ghost1, ghost2, ghost3, ghost4};


    check_collision_with_ghosts(ghosts, square);

    return G_SOURCE_CONTINUE;
}

double calculateDistance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void showInfoDialog(const char* infoMessage) {
    GtkWidget* dialog;
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, 
    GTK_BUTTONS_OK, infoMessage);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void quitApplication() {
    cleanup();
    g_application_quit(G_APPLICATION(app));
}

void setColor(GtkWidget* widget, gpointer userData) {
    if (cssProvider == NULL) {
        cssProvider = gtk_css_provider_new();
        GError* error = NULL;

        if (!gtk_css_provider_load_from_path(cssProvider, "style.css", &error)) {
            g_error(COLOR_RED "Error loading CSS file: %s" COLOR_RESET , error->message);
            g_clear_error(&error);
        }

        gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), 
            GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[1;1H");
#endif
}

void cleanup() {
    g_source_remove(labelUpdateTimeoutId);
}

void startTimerGhosts() {
    timer->start = clock();
}

double getElapsedTimeGhosts() {
    return ((double)(clock() - timer->start)) / CLOCKS_PER_SEC;
}

int getOppositeDirection(int direction) {
    switch (direction) {
        case UP:
            return DOWN;
        case RIGHT:
            return LEFT;
        case DOWN:
            return UP;
        case LEFT:
            return RIGHT;
        default:
            return -1; // Invalid direction
    }
}

Wall walls[NUM_WALLS] = {
    {0, 0, 1200, 10, {33/255.0, 33/255.0, 222/255.0}},
    {0, 330, 1200, 10, {33/255.0, 33/255.0, 222/255.0}},
    {0, 10, 10, 100, {33/255.0, 33/255.0, 222/255.0}},
    {1190, 10, 10, 100, {33/255.0, 33/255.0, 222/255.0}},
    {0, 230, 10, 100, {33/255.0, 33/255.0, 222/255.0}},
    {1190, 230, 10, 100, {33/255.0, 33/255.0, 222/255.0}},
    {0, 110, 90, 10, {33/255.0, 33/255.0, 222/255.0}},
    {80, 120, 10, 30, {33/255.0, 33/255.0, 222/255.0}},
    {0, 140, 80, 10, {33/255.0, 33/255.0, 222/255.0}},
    {0, 220, 90, 10, {33/255.0, 33/255.0, 222/255.0}},
    {80, 190, 10, 30, {33/255.0, 33/255.0, 222/255.0}},
    {0, 190, 80, 10, {33/255.0, 33/255.0, 222/255.0}},
    {1110, 110, 90, 10, {33/255.0, 33/255.0, 222/255.0}},
    {1110, 120, 10, 30, {33/255.0, 33/255.0, 222/255.0}},
    {1120, 140, 80, 10, {33/255.0, 33/255.0, 222/255.0}},
    {1110, 220, 90, 10, {33/255.0, 33/255.0, 222/255.0}},
    {1110, 190, 10, 30, {33/255.0, 33/255.0, 222/255.0}},
    {1120, 190, 80, 10, {33/255.0, 33/255.0, 222/255.0}},
    {50, 50, 40, 20, {33/255.0, 33/255.0, 222/255.0}},
    {50, 270, 60, 20, {33/255.0, 33/255.0, 222/255.0}},
    {1070, 50, 80, 20, {33/255.0, 33/255.0, 222/255.0}},
    {1070, 270, 80, 20, {33/255.0, 33/255.0, 222/255.0}},
    {950, 50, 80, 20, {33/255.0, 33/255.0, 222/255.0}},
    {870, 270, 160, 20, {33/255.0, 33/255.0, 222/255.0}},
    {150, 270, 200, 20, {33/255.0, 33/255.0, 222/255.0}},
    {450, 270, 200, 20, {33/255.0, 33/255.0, 222/255.0}},
    {350, 50, 300, 20, {33/255.0, 33/255.0, 222/255.0}},
    {690, 50, 140, 20, {33/255.0, 33/255.0, 222/255.0}},
    {390, 270, 20, 60, {33/255.0, 33/255.0, 222/255.0}},
    {690, 290, 20, 40, {33/255.0, 33/255.0, 222/255.0}},
    {810, 290, 20, 40, {33/255.0, 33/255.0, 222/255.0}},
    {690, 230, 140, 20, {33/255.0, 33/255.0, 222/255.0}},
    {750, 250, 20, 40, {33/255.0, 33/255.0, 222/255.0}},
    {130, 50, 180, 40, {33/255.0, 33/255.0, 222/255.0}},
    {130, 90, 40, 140, {33/255.0, 33/255.0, 222/255.0}},
    {170, 190, 140, 40, {33/255.0, 33/255.0, 222/255.0}},
    {270, 150, 40, 40, {33/255.0, 33/255.0, 222/255.0}},
    {210, 130, 100, 20, {33/255.0, 33/255.0, 222/255.0}},
    {350, 110, 130, 120, {253/255.0, 0, 0}},
    {390, 150, 50, 40, {253/255.0, 0, 0}},
    {520, 110, 130, 120, {1, 1, 0}},
    {560, 150, 50, 40, {1, 1, 0}},
    {870, 50, 40, 180, {0, 1, 0}},
    {950, 110, 120, 60, {253/255.0, 0, 0}},
    {990, 130, 40, 20, {253/255.0, 0, 0}},
    {950, 170, 40, 60, {253/255.0, 0, 0}},
    {990, 210, 80, 20, {253/255.0, 0, 0}},
    {690, 110, 10, 80, {33/255.0, 33/255.0, 222/255.0}},
    {700, 180, 120, 10, {33/255.0, 33/255.0, 222/255.0}},
    {820, 110, 10, 80, {33/255.0, 33/255.0, 222/255.0}},
    {700, 110, 40, 10, {33/255.0, 33/255.0, 222/255.0}},
    {780, 110, 40, 10, {33/255.0, 33/255.0, 222/255.0}},
    {740, 110, 40, 10, {222/255.0, 161/255.0, 133/255.0}} // Special Wall
    // E wall for ghosts {1069, 170, 1, 40}
    // G wall for ghosts {309, 90, 1, 40}
};

// Mini size-> 6
// Big size-> 18
// Min - Min distance -> 15.5
// Min - Big distance-> 8
// Min - Wall distance-> 16 ~ 15.5
// Big - Wall distance-> 10

Point points[NUM_POINTS] = {
    // Fruit
    {750, 200, 20, 20, true},
    // Big
    {20, 302, 18, 18, false},
    {1162, 302, 18, 18, false},
    {20, 66, 18, 18, false},
    {1162, 66, 18, 18, false},
    {235, 164, 18, 18, false},
    // Small
    // Top line
    {25.5, 25.5, 6, 6, false},
    {47, 25.5, 6, 6, false},
    {68.5, 25.5, 6, 6, false},
    {90, 25.5, 6, 6, false},
    {111.5, 25.5, 6, 6, false},
    {133, 25.5, 6, 6, false},
    {154.5, 25.5, 6, 6, false},
    {176, 25.5, 6, 6, false},
    {197, 25.5, 6, 6, false},
    {219, 25.5, 6, 6, false},
    {240.5, 25.5, 6, 6, false},
    {262, 25.5, 6, 6, false},
    {283.5, 25.5, 6, 6, false},
    {305, 25.5, 6, 6, false},
    {326.5, 25.5, 6, 6, false},
    {348, 25.5, 6, 6, false},
    {369.5, 25.5, 6, 6, false},
    {391, 25.5, 6, 6, false},
    {412.5, 25.5, 6, 6, false},
    {434, 25.5, 6, 6, false},
    {455.5, 25.5, 6, 6, false},
    {477, 25.5, 6, 6, false},
    {498.5, 25.5, 6, 6, false},
    {521, 25.5, 6, 6, false},
    {542.5, 25.5, 6, 6, false},
    {564, 25.5, 6, 6, false},
    {585.5, 25.5, 6, 6, false},
    {607, 25.5, 6, 6, false},
    {628.5, 25.5, 6, 6, false},
    {650, 25.5, 6, 6, false},
    {671.5, 25.5, 6, 6, false},
    {693, 25.5, 6, 6, false},
    {714.5, 25.5, 6, 6, false},
    {736, 25.5, 6, 6, false},
    {757.5, 25.5, 6, 6, false},
    {779, 25.5, 6, 6, false},
    {800.5, 25.5, 6, 6, false},
    {822, 25.5, 6, 6, false},
    {843.5, 25.5, 6, 6, false},
    {865, 25.5, 6, 6, false},
    {886.5, 25.5, 6, 6, false},
    {908, 25.5, 6, 6, false},
    {929.5, 25.5, 6, 6, false},
    {951, 25.5, 6, 6, false},
    {972.5, 25.5, 6, 6, false},
    {994, 25.5, 6, 6, false},
    {1013.5, 25.5, 6, 6, false},
    {1033, 25.5, 6, 6, false},
    {1052.5, 25.5, 6, 6, false},
    {1072, 25.5, 6, 6, false},
    {1105.5, 25.5, 6, 6, false},
    {1126, 25.5, 6, 6, false},
    {1146.5, 25.5, 6, 6, false},
    {1167, 25.5, 6, 6, false},

    // Middle top line

    {25.5, 92, 6, 6, false},
    {47, 92, 6, 6, false},
    {68.5, 92, 6, 6, false},
    {90, 92, 6, 6, false},
    {111.5, 92, 6, 6, false},
    {326.5, 92, 6, 6, false},
    {348, 92, 6, 6, false},
    {369.5, 92, 6, 6, false},
    {391, 92, 6, 6, false},
    {412.5, 92, 6, 6, false},
    {434, 92, 6, 6, false},
    {455.5, 92, 6, 6, false},
    {477, 92, 6, 6, false},
    {498.5, 92, 6, 6, false},
    {521, 92, 6, 6, false},
    {542.5, 92, 6, 6, false},
    {564, 92, 6, 6, false},
    {585.5, 92, 6, 6, false},
    {607, 92, 6, 6, false},
    {628.5, 92, 6, 6, false},
    {650, 92, 6, 6, false},
    {671.5, 92, 6, 6, false},
    {929.5, 92, 6, 6, false},
    {951, 92, 6, 6, false},
    {972.5, 92, 6, 6, false},
    {994, 92, 6, 6, false},
    {1013.5, 92, 6, 6, false},
    {1033, 92, 6, 6, false},
    {1052.5, 92, 6, 6, false},
    {1072, 92, 6, 6, false},
    {1105.5, 92, 6, 6, false},
    {1126, 92, 6, 6, false},
    {1146.5, 92, 6, 6, false},
    {1167, 92, 6, 6, false},
    
    // Bottom line

    {46, 310.5, 6, 6, false},
    {67.5, 310.5, 6, 6, false},
    {89, 310.5, 6, 6, false},
    {110.5, 310.5, 6, 6, false},
    {132, 310.5, 6, 6, false},
    {153.5, 310.5, 6, 6, false},
    {175, 310.5, 6, 6, false},
    {196.5, 310.5, 6, 6, false},
    {218, 310.5, 6, 6, false},
    {239.5, 310.5, 6, 6, false},
    {260, 310.5, 6, 6, false},
    {280.5, 310.5, 6, 6, false},
    {301, 310.5, 6, 6, false},
    {321.5, 310.5, 6, 6, false},
    {342, 310.5, 6, 6, false},
    {362.5, 310.5, 6, 6, false},
    {417.5, 310.5, 6, 6, false},
    {438, 310.5, 6, 6, false},
    {458.5, 310.5, 6, 6, false},
    {479, 310.5, 6, 6, false},
    {498, 310.5, 6, 6, false},
    {518.5, 310.5, 6, 6, false},
    {539, 310.5, 6, 6, false},
    {559.5, 310.5, 6, 6, false},
    {580, 310.5, 6, 6, false},
    {600.5, 310.5, 6, 6, false},
    {621, 310.5, 6, 6, false},
    {641.5, 310.5, 6, 6, false},
    {662, 310.5, 6, 6, false},
    {717, 310.5, 6, 6, false},
    {793, 310.5, 6, 6, false},
    {848, 310.5, 6, 6, false},
    {868.5, 310.5, 6, 6, false},
    {889, 310.5, 6, 6, false},
    {909.5, 310.5, 6, 6, false},
    {929, 310.5, 6, 6, false},
    {946.5, 310.5, 6, 6, false},
    {966, 310.5, 6, 6, false},
    {987.5, 310.5, 6, 6, false},
    {1007, 310.5, 6, 6, false},
    {1028.5, 310.5, 6, 6, false},
    {1048, 310.5, 6, 6, false},
    {1069.5, 310.5, 6, 6, false},
    {1089.5, 310.5, 6, 6, false},
    {1110, 310.5, 6, 6, false},
    {1130.5, 310.5, 6, 6, false},
    {1151, 310.5, 6, 6, false},

    // Middle bottom line

    {25.5, 246, 6, 6, false},
    {46, 246, 6, 6, false},
    {67.5, 246, 6, 6, false},
    {89, 246, 6, 6, false},
    {110.5, 246, 6, 6, false},
    {132, 246, 6, 6, false},
    {153.5, 246, 6, 6, false},
    {175, 246, 6, 6, false},
    {196.5, 246, 6, 6, false},
    {218, 246, 6, 6, false},
    {239.5, 246, 6, 6, false},
    {260, 246, 6, 6, false},
    {280.5, 246, 6, 6, false},
    {301, 246, 6, 6, false},
    {321.5, 246, 6, 6, false},
    {342, 246, 6, 6, false},
    {362.5, 246, 6, 6, false},
    {382, 246, 6, 6, false},
    {400.5, 246, 6, 6, false},
    {420.5, 246, 6, 6, false},
    {438, 246, 6, 6, false},
    {458.5, 246, 6, 6, false},
    {477, 246, 6, 6, false},
    {498.5, 246, 6, 6, false},
    {518.5, 246, 6, 6, false},
    {537, 246, 6, 6, false},
    {557.5, 246, 6, 6, false},
    {578, 246, 6, 6, false},
    {598.5, 246, 6, 6, false},
    {619, 246, 6, 6, false},
    {639.5, 246, 6, 6, false},
    {661, 246, 6, 6, false},
    {848, 246, 6, 6, false},
    {868.5, 246, 6, 6, false},
    {889, 246, 6, 6, false},
    {909.5, 246, 6, 6, false},
    {929.5, 246, 6, 6, false},
    {946, 246, 6, 6, false},
    {966.5, 246, 6, 6, false},
    {987, 246, 6, 6, false},
    {1007.5, 246, 6, 6, false},
    {1028, 246, 6, 6, false},
    {1048.5, 246, 6, 6, false},
    {1069, 246, 6, 6, false},
    {1089.5, 246, 6, 6, false},
    {1110, 246, 6, 6, false},
    {1130.5, 246, 6, 6, false},
    {1151, 246, 6, 6, false},
    {1170.5, 246, 6, 6, false},

    // L-E column

    {929.5, 47, 6, 6, false},
    {929.5, 68.5, 6, 6, false},
    {929.5, 109.5, 6, 6, false},
    {929.5, 129, 6, 6, false},
    {929.5, 149.5, 6, 6, false},
    {929.5, 170, 6, 6, false},
    {929.5, 190.5, 6, 6, false},
    {929.5, 211, 6, 6, false},
    {929.5, 231.5, 6, 6, false},

    // E-border column

    {1089.5, 24.5, 6, 6, false},
    {1089.5, 92, 6, 6, false},
    {1089.5, 109.5, 6, 6, false},
    {1089.5, 129, 6, 6, false},
    {1089.5, 149.5, 6, 6, false},
    {1089.5, 170, 6, 6, false},
    {1089.5, 190.5, 6, 6, false},
    {1089.5, 211, 6, 6, false},
    {1089.5, 231.5, 6, 6, false},

    //O-O column

    {498.5, 111.5, 6, 6, false},
    {498.5, 131, 6, 6, false},
    {498.5, 150.5, 6, 6, false},
    {498.5, 170, 6, 6, false},
    {498.5, 189.5, 6, 6, false},
    {498.5, 209, 6, 6, false},
    {498.5, 226.5, 6, 6, false},

    //border-G column

    {111.5, 47, 6, 6, false},
    {111.5, 68.5, 6, 6, false},
    {111.5, 111.5, 6, 6, false},
    {111.5, 131, 6, 6, false},
    {111.5, 150.5, 6, 6, false},
    {111.5, 170, 6, 6, false},
    {111.5, 189.5, 6, 6, false},
    {111.5, 209, 6, 6, false},
    {111.5, 226.5, 6, 6, false},

    //G-O column
    {326.5, 47, 6, 6, false},
    {326.5, 68.5, 6, 6, false},
    {326.5, 111.5, 6, 6, false},
    {326.5, 131, 6, 6, false},
    {326.5, 150.5, 6, 6, false},
    {326.5, 170, 6, 6, false},
    {326.5, 189.5, 6, 6, false},
    {326.5, 209, 6, 6, false},
    {326.5, 226.5, 6, 6, false},

    //Additional

    {25.5, 47, 6, 6, false},
    {671.5, 47, 6, 6, false},
    {671.5, 68.5, 6, 6, false},
    {1054.5, 47, 6, 6, false},
    {1054.5, 68.5, 6, 6, false},
    {1167, 47, 6, 6, false},
    {25.5, 267.5, 6, 6, false},
    {25.5, 288, 6, 6, false},
    {132, 267.5, 6, 6, false},
    {132, 288, 6, 6, false},
    {362.5, 267.5, 6, 6, false},
    {362.5, 288, 6, 6, false},
    {417.5, 267.5, 6, 6, false},
    {417.5, 288, 6, 6, false},
    {662, 267.5, 6, 6, false},
    {662, 288, 6, 6, false},
    {717, 267.5, 6, 6, false},
    {717, 288, 6, 6, false},
    {793, 267.5, 6, 6, false},
    {793, 288, 6, 6, false},
    {848, 267.5, 6, 6, false},
    {848, 288, 6, 6, false},
    {1048.5, 267.5, 6, 6, false},
    {1048.5, 288, 6, 6, false},
    {1170.5, 267.5, 6, 6, false},
    {1170.5, 288, 6, 6, false},
    {680, 267.5, 6, 6, false},
    {700, 267.5, 6, 6, false},
    {810, 267.5, 6, 6, false},
    {830, 267.5, 6, 6, false},

    //E inside

    {1068, 190.5, 6, 6, false},
    {1046.5, 190.5, 6, 6, false},
    {1025, 190.5, 6, 6, false},
    {1003.5, 190.5, 6, 6, false},

    //G inside

    {303.5, 111.5, 6, 6, false},
    {282, 111.5, 6, 6, false},
    {260.5, 111.5, 6, 6, false},
    {239, 111.5, 6, 6, false},
    {217.5, 111.5, 6, 6, false},
    {196, 111.5, 6, 6, false},
    {196, 131, 6, 6, false},
    {196, 150.5, 6, 6, false},
    {196, 170, 6, 6, false},
    {217.5, 170, 6, 6, false},
};
