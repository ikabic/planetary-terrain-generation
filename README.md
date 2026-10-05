<h1 align="center">
  <br>
   <a href="/">
     <img width="1800" height="222" alt="banner" src="https://github.com/user-attachments/assets/94fcfec7-3955-4165-a643-d3c01bb53aec" />
</a>
  <br>
</h1>

<p align="center">
  <a href="">
    <img src="https://img.shields.io/badge/Watch%20Demo-FF0000?style=for-the-badge&logo=youtube&logoColor=white">
  </a>
</p>

<div align="center">

[![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/17)
[![OpenGL](https://img.shields.io/badge/OpenGL-3D-5586A4?logo=opengl&logoColor=white)](https://www.opengl.org/)
[![FastNoiseLite](https://img.shields.io/badge/FastNoiseLite-Procedural%20Noise-6C5CE7)](https://github.com/Auburn/FastNoiseLite)
[![Dear ImGui](https://img.shields.io/badge/Dear%20ImGui-UI-000000)](https://github.com/ocornut/imgui)

</div>

<p align="center">
  A C++ procedural planet generator that creates diverse virtual planets and their circumplanetary systems using deterministic, noise-based terrain generation. The program was developed as part of an undergraduate thesis on the topic:
  <br>
  <strong>"Procedural planet generation using cube spherisation"</strong>
</p>

## Table of Contents
- [Features](#features)
- [Tech Stack](#tech-stack)
- [VS Filter Structure](#vs-filter-structure)
- [Installation](#installation)

## Features
- **Seed manipulation**: automatic randomisation and manual text field input.

- **View export**: export planet image via button.

- **View manipulation**: control view and zoom with WASD/arrow keys and mouse scroll wheel.

- **Stylisation**: retro pixel art design.

### Potential future features

- **View upgrades:** visibility toggles for circumplanetary elements, motion toggle.
  
- **Planet additions:** atmosphere generation, description panel (name, short description, temperature, habitability status, etc.).
  
- **Animation:** short animation when switching between planets.

- **Ambience:** background ambient sound generation.

## Tech Stack
💻 **Language:** C++17

⚙️ **Build system:** CMake

🖼️ **Graphics:** OpenGL, GLFW, GLEW, GLM

🪐 **Procedural Generation:** FastNoiseLite

👤 **User Interface:** Dear ImGUI

📦 **Data & Assets:** nlohmann/json, stb

## VS Filter Structure
📦 PlanetaryTerrainGeneration<br />
 ┣ 🗂️ Header Files - contains .h files<br />
 ┃ ┣ 📂 Core - utility files and global vars<br />
 ┃ ┣ 📂 Graphics - OpenGL shaders and rendering, and pixelisation<br />
 ┃ ┣ 📂 Planet<br />
 ┃ ┃ ┣ 📂 Terrain - planet terrain generation<br />
 ┃ ┃ ┣ 📂 Palette - planet surface colour generation<br />
 ┃ ┃ ┗ 📂 Traits - planet's circumplanetary system generation<br />
 ┃ ┗ 📂 UI - view setup and graphical interface<br />
 ┗ 🗂️ Source Files - contains .cpp files with the same structure as header files + shader folder<br />

## Installation

To run the program download the latest release:

1. Go to the [**Releases**](../../releases) page.
2. Download the latest `.zip` archive.
3. Extract the archive.
4. Run `PlanetaryTerrainGeneration.exe`.

The release archive contains the executable and all required assets.

**Note:** Windows may display a security warning when running the executable because the application is not digitally signed. If you trust the downloaded release, select **More info → Run anyway** to launch the application.
