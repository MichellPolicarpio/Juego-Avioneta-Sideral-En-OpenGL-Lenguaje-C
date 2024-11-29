#ifndef PLAYER_H
#define PLAYER_H

#include "config.h"
#include <stdbool.h>

#define MAX_TRAIL_PARTICLES 50  // Número máximo de partículas en la estela

// Estructura para las partículas de la estela
typedef struct {
    float x, y;        // Posición
    float alpha;       // Transparencia
    float size;        // Tamaño
} TrailParticle;

// Estructura para el estado del jugador (avioneta)
typedef struct {
    float y_pos;           // Posición vertical
    float y_velocity;      // Velocidad vertical
    float normal_speed;    // Velocidad normal
    float boost_speed;     // Velocidad con boost
    float current_speed;   // Velocidad actual
    float rotation;        // Rotación de la avioneta
    
    struct {
        float angle;
        float speed;
    } propeller;
    
    float max_y;          // Límite superior
    float min_y;          // Límite inferior

    // Añadir estela
    TrailParticle trail[MAX_TRAIL_PARTICLES];
    int trail_count;
} Player;

// Variables externas
extern Player player;

// Prototipos de funciones
void init_player(void);
void update_player(float delta_time);
void render_player(void);
void handle_player_input(unsigned char key);
void handle_player_key_up(unsigned char key);
void update_propeller(void);
void init_trail(void);
void update_trail(float delta_time);
void render_trail(void);

#endif // PLAYER_H