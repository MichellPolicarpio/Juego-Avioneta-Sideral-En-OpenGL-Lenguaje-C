# 🎮 Juego Avioneta Sideral en OpenGL/C

![Banner del Juego](insertar_imagen_banner.png)

## 📖 Descripción
Juego de acción en 2D/3D desarrollado en C utilizando OpenGL. El jugador controla una avioneta a través de terreno hostil, evadiendo obstáculos, recolectando municiones y completando misiones tácticas.

### ✨ Características Principales
- 🛩️ Control fluido de la avioneta con física básica
- 💥 Sistema de combate con municiones recolectables
- 🎯 Sistema de misiones progresivas
- 💫 Efectos visuales con partículas
- 💾 Sistema de guardado de progreso
- 📊 Múltiples niveles de dificultad
- 🎨 Interfaz dividida en tres ventanas informativas

## 🛠️ Tecnologías Utilizadas
- OpenGL
- GLUT
- Lenguaje C
- Bibliotecas estándar de C

## 🎲 Estructuras de Datos Implementadas
- 📝 Lista enlazada (Sistema de obstáculos)
- 🌳 Árbol binario (Sistema de diálogos)
- 📚 Cola (Sistema de misiones)
- 📦 Pila (Sistema de guardado)

## 📋 Requisitos Previos
- Sistema Operativo: MacOS
- OpenGL instalado
- GLUT instalado
- Compilador de C

## ⚙️ Instalación

1. Clonar el repositorio:
```bash
git clone https://github.com/MichellPolicarpio/Juego-Avioneta-Sideral-En-OpenGL-Lenguaje-C.git
```

2. Navegar al directorio:
```bash
cd Juego-Avioneta-Sideral-En-OpenGL-Lenguaje-C
```

3. Compilar el juego:
```bash
gcc -Wall -framework OpenGL -framework GLUT main.c -o juego
```

4. Ejecutar:
```bash
./juego
```

## 🎮 Controles

| Tecla | Acción |
|-------|--------|
| W / ↑ | Ascender |
| S / ↓ | Descender |
| Espacio | Activar propulsor |
| B | Disparar |
| P | Pausar |
| ESC | Menú/Pausa |

## 🎯 Objetivos del Juego
1. Navegar a través de obstáculos
2. Recolectar municiones
3. Completar misiones tácticas
4. Lograr la máxima puntuación

## 🖼️ Capturas de Pantalla

![Gameplay](insertar_imagen_gameplay.png)
![Menú Principal](insertar_imagen_menu.png)
![Terminal](insertar_imagen_terminal.png)

## 🏗️ Arquitectura del Proyecto

```
proyecto/
├── main.c
├── config.h
├── stb_image.h
├── assets/
│   ├── texturas/
│   └── sonidos/
└── docs/
    └── manual.md
```

## 📈 Características Técnicas
- Sistema de partículas para efectos visuales
- Detección de colisiones optimizada
- Generación procedural de obstáculos
- Sistema de diálogos ramificados
- Gestión de estados de juego

## 🤝 Contribución
Las contribuciones son bienvenidas. Para cambios importantes:
1. Fork el proyecto
2. Cree una rama para su característica
3. Commit sus cambios
4. Push a la rama
5. Abra un Pull Request

## 📝 Licencia
Este proyecto está bajo la Licencia MIT - vea el archivo [LICENSE.md](LICENSE.md) para detalles.

## ✍️ Autor
**Michell Policarpio** - [GitHub](https://github.com/MichellPolicarpio)
- Matrícula: zS21002379
- Universidad Veracruzana
- Facultad de Ingeniería Eléctrica y Electrónica

## 🎓 Contexto Académico
Proyecto Final para la materia de Graficación por Computadora
- **Universidad:** Universidad Veracruzana
- **Facultad:** FIEE
- **Período:** 2024
- **Profesor:** [Nombre del Profesor]
- **NRC:** 18679

## 🔗 Enlaces de Interés
- [Documentación de OpenGL](http://docs.gl/)
- [Tutorial Original](link_al_tutorial)
- [Manual de Usuario](docs/manual.md)

## 🙏 Agradecimientos
- Universidad Veracruzana por el apoyo y recursos
- Profesor por la guía durante el desarrollo
- Comunidad de OpenGL por la documentación y ejemplos

---
Desarrollado con ❤️ por Michell Policarpio
