#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "config.h"
#include "player.h"  // Añadimos el include de player
#include <stdbool.h>
#include <math.h>

#define MAX_OBSTACLES 200

typedef struct {
    float x;          // Posición X
    float y;          // Posición Y
    float width;      // Ancho del obstáculo
    float height;     // Alto del obstáculo
    bool active;      // Si el obstáculo está activo
} Obstacle;

typedef struct {
    Obstacle obstacles[MAX_OBSTACLES];
    float spawn_timer;         // Tiempo para siguiente obstáculo
    float spawn_interval;      // Intervalo entre obstáculos
    float speed;              // Velocidad base de movimiento
    int active_count;         // Número de obstáculos activos
} ObstacleSystem;

// Variables externas
extern ObstacleSystem obstacle_system;

// Funciones
void init_obstacles(void);
void update_obstacles(float delta_time);
void render_obstacles(void);
bool check_collision(float player_x, float player_y, float player_radius);
void spawn_obstacle(void);

#endif // OBSTACLE_H