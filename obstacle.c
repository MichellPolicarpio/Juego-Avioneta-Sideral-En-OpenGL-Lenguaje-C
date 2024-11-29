#include "obstacle.h"
#include <stdlib.h>

ObstacleSystem obstacle_system;

void init_obstacles(void) {
    obstacle_system.spawn_timer = 0.0f;
    obstacle_system.spawn_interval = 1.0f;
    obstacle_system.speed = 30.0f;  // Esta será la velocidad base
    obstacle_system.active_count = 0;

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacle_system.obstacles[i].active = false;
    }
}

void spawn_obstacle(void) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacle_system.obstacles[i].active) {
            Obstacle* obs = &obstacle_system.obstacles[i];
            obs->x = 100.0f;
            obs->y = -15.0f + (rand() % 300) / 10.0f;
            obs->width = 2.0f;
            obs->height = 8.0f + (rand() % 40) / 10.0f;
            obs->active = true;
            obstacle_system.active_count++;
            break;
        }
    }
}

void update_obstacles(float delta_time) {
    // Actualizar timer para spawn
    obstacle_system.spawn_timer += delta_time;
    if (obstacle_system.spawn_timer >= obstacle_system.spawn_interval) {
        spawn_obstacle();
        obstacle_system.spawn_timer = 0.0f;
    }

    // Actualizar posiciones usando la velocidad del jugador
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacle_system.obstacles[i].active) {
            // Usar la velocidad actual del jugador
            obstacle_system.obstacles[i].x -= player.current_speed * delta_time;

            if (obstacle_system.obstacles[i].x < -50.0f) {
                obstacle_system.obstacles[i].active = false;
                obstacle_system.active_count--;
            }
        }
    }
}

void render_obstacles(void) {
    glColor3f(1.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacle_system.obstacles[i].active) {
            glPushMatrix();
                glTranslatef(obstacle_system.obstacles[i].x,
                           obstacle_system.obstacles[i].y,
                           0.0f);
                glScalef(obstacle_system.obstacles[i].width,
                        obstacle_system.obstacles[i].height,
                        1.0f);
                glutSolidCube(1.0f);
            glPopMatrix();
        }
    }
}

bool check_collision(float player_x, float player_y, float player_radius) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacle_system.obstacles[i].active) {
            float dx = player_x - obstacle_system.obstacles[i].x;
            float dy = player_y - obstacle_system.obstacles[i].y;
            float distance = sqrt(dx * dx + dy * dy);
            
            if (distance < player_radius + obstacle_system.obstacles[i].width) {
                return true;
            }
        }
    }
    return false;
}