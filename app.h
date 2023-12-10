#ifndef APP_H
#define APP_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <glib.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RESET "\x1b[0m"
#define SQUARE_SIZE 40
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 340
#define BORDER 4
#define SPEED 10
#define SELF_SPEED 50
#define TURNING_TOLERANCE 40
#define NUM_WALLS 53
#define NUM_POINTS 277
#define NUM_PORTALS 2


extern GtkApplication* app;

enum Direction { UP, DOWN, LEFT, RIGHT, STAY };

typedef struct {
    double x;
    double y;
} Entity;

typedef struct {
    double x;
    double y;
    char name[10];
    char nickName[10];
    enum Direction currentDirection;
    Entity entity;
} Square;

typedef struct {
    double x;
    double y;
    double ghostSpeed;
    char name[10];
    char nickName[10];
    double scatterX;
    double scatterY;
    double frightenedModeTimeout;
    time_t frightenedModeStart;
    bool frightenedModeActive;
    enum Direction currentDirection;
    Entity entity;
} Ghost;

typedef struct {
    double x;
    double y;
} Velocity;

typedef struct {
    double x;
    double y;
    double width;
    double height;
    bool isCollected;
} Point;

typedef struct {
    double x;
    double y;
    double width;
    double height;
    double color[3];
} Wall;

typedef struct {
    clock_t start;
} Timer;


extern enum Direction currentDirection;
extern enum Direction nextDirection;
extern enum Direction requestedDirection;

extern unsigned int stayedPoints;
extern unsigned int totalPoints;
extern unsigned int currentLevel;
extern unsigned int timeout;
extern const unsigned int maxLevel;
extern clock_t startTime;
extern guint labelUpdateTimeoutId;
extern GtkCssProvider *cssProvider;
extern GtkWidget *window;
extern GtkWidget *grid;
extern GtkWidget *drawing_area;
extern GtkWidget *label;
extern time_t enterTimePinky;
extern time_t enterTimeInky;
extern time_t enterTimeClyde;
extern time_t enterTimeBlinky;
extern bool newPointCollectedBlinky;
extern bool newPointCollectedPinky;
extern bool newPointCollectedInky;
extern bool newPointCollectedClyde;
extern bool inBoxPinky;
extern bool inBoxInky;
extern bool inBoxClyde;
extern bool inBoxBlinky;
extern bool outBoxPinky;
extern bool outBoxInky;
extern bool outBoxClyde;
extern bool outBoxBlinky;
extern Square* square;
extern Ghost* ghost1;
extern Ghost* ghost2;
extern Ghost* ghost3;
extern Ghost* ghost4;
extern Timer* timer;
extern const GdkRGBA PACMAN_COLOR;
extern const GdkRGBA GHOST_COLOR_1;
extern const GdkRGBA GHOST_COLOR_2;
extern const GdkRGBA GHOST_COLOR_3;
extern const GdkRGBA GHOST_COLOR_4;
extern const GdkRGBA GHOST_COLOR_F;
extern Wall walls [NUM_WALLS];
extern Point points [NUM_POINTS];


gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data);
gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean key_press_timeout_callback(gpointer user_data);
gboolean updateLabelPeriodically(gpointer user_data);
double distanceToWall(Square *square, enum Direction direction);
void draw_figure(cairo_t *cr, void *figure);
void activate(GtkApplication *app, gpointer user_data);
void printTitle();
void printSlowly(const char *str);
void draw_walls(cairo_t *cr, Wall *walls, int num_walls);
bool check_collision(Entity *entity, Wall *walls, int num_walls);
bool check_collision(Entity *entity, Wall *walls, int num_walls);
bool special_check_collision(Square *square, Wall *walls, int num_walls, enum Direction direction);
bool check_collision_with_points(Square *square, Point *points, int num_points);
int check_portal(Entity *entity);
double getElapsedTime();
int getOppositeDirection(int direction);
void updateLabelCallback(GtkWidget *label);
void showInfoDialog(const char* infoMessage);
void showResultDialog(const char* resultMessage);
void quitApplication();
void setColor(GtkWidget* widget, gpointer userData);
void clearScreen();
void cleanup();
void moveBlinky(Ghost *blinky, Square *pacman);
void movePinky(Ghost *pinky, Square *pacman);
void moveInky(Ghost *inky, Ghost *blinky, Square *pacman);
void moveClyde(Ghost *clyde, Square *pacman);
double calculateDistance(double x1, double y1, double x2, double y2);
bool check_collision_with_walls(double x, double y, double width, double height);
double getElapsedTimeGhosts();
void startTimerGhosts();
bool checkGhostCollision(Ghost *ghost, Square *pacman);
void check_collision_with_ghosts(Ghost* ghosts[4], Square* pacman);
void ghostEaten(Ghost* ghost);
void changeGhostColor(cairo_t *cr, Ghost *ghost);

#endif // APP_H