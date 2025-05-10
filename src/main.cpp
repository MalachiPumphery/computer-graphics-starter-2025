#ifdef _WIN32
#include <windows.h>
#endif
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include "Canis/Canis.hpp"
#include "Canis/Entity.hpp"
#include "Canis/Graphics.hpp"
#include "Canis/Window.hpp"
#include "Canis/Shader.hpp"
#include "Canis/Debug.hpp"
#include "Canis/IOManager.hpp"
#include "Canis/InputManager.hpp"
#include "Canis/Camera.hpp"
#include "Canis/Model.hpp"
#include "Canis/World.hpp"
#include "Canis/Editor.hpp"
#include "Canis/FrameRateManager.hpp"
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For seeding rand()

using namespace glm;

// git restore .
// git fetch
// git pull

// 3d array
std::vector<std::vector<std::vector<unsigned int>>> map = {};

// declaring functions
void SpawnLights(Canis::World &_world);
void LoadMap(std::string _path);
void Rotate(Canis::World &_world, Canis::Entity &_entity, float _deltaTime);

#ifdef _WIN32
#define main SDL_main
extern "C" int main(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    Canis::Init();
    Canis::InputManager inputManager;
    Canis::FrameRateManager frameRateManager;
    frameRateManager.Init(60);

    /// SETUP WINDOW
    Canis::Window window;
    window.MouseLock(true);

    unsigned int flags = 0;

    if (Canis::GetConfig().fullscreen)
        flags |= Canis::WindowFlags::FULLSCREEN;

    window.Create("Hello Graphics", Canis::GetConfig().width, Canis::GetConfig().heigth, flags);
    /// END OF WINDOW SETUP

    Canis::World world(&window, &inputManager, "assets/textures/lowpoly-skybox/");
    SpawnLights(world);

    Canis::Editor editor(&window, &world, &inputManager);

    Canis::Graphics::EnableAlphaChannel();
    Canis::Graphics::EnableDepthTest();

    /// SETUP SHADER
    Canis::Shader shader;
    shader.Compile("assets/shaders/hello_shader.vs", "assets/shaders/hello_shader.fs");
    shader.AddAttribute("aPosition");
    shader.Link();
    shader.Use();
    shader.SetInt("MATERIAL.diffuse", 0);
    shader.SetInt("MATERIAL.specular", 1);
    shader.SetFloat("MATERIAL.shininess", 64);
    shader.SetBool("WIND", false);
    shader.UnUse();

    Canis::Shader grassShader;
    grassShader.Compile("assets/shaders/hello_shader.vs", "assets/shaders/hello_shader.fs");
    grassShader.AddAttribute("aPosition");
    grassShader.Link();
    grassShader.Use();
    grassShader.SetInt("MATERIAL.diffuse", 0);
    grassShader.SetInt("MATERIAL.specular", 1);
    grassShader.SetFloat("MATERIAL.shininess", 64);
    grassShader.SetBool("WIND", true);
    grassShader.SetFloat("WINDEFFECT", 0.2);
    grassShader.UnUse();
    /// END OF SHADER

    /// Load Image
    Canis::GLTexture glassTexture = Canis::LoadImageGL("assets/textures/glass.png", true);
    Canis::GLTexture grassTexture = Canis::LoadImageGL("assets/textures/grass_block_top.png", false);
    Canis::GLTexture blueFlowerTexture = Canis::LoadImageGL("assets/textures/blue_orchid.png", false);
    Canis::GLTexture textureSpecular = Canis::LoadImageGL("assets/textures/container2_specular.png", true);
    Canis::GLTexture oakLogTexture = Canis::LoadImageGL("assets/textures/oak_log.png", false);
    Canis::GLTexture oakPlanksTexture = Canis::LoadImageGL("assets/textures/oak_planks.png", false);
    Canis::GLTexture cobblestoneTexture = Canis::LoadImageGL("assets/textures/cobblestone.png", false);
    Canis::GLTexture bricksTexture = Canis::LoadImageGL("assets/textures/bricks.png", false);
    Canis::GLTexture grassSideTexture = Canis::LoadImageGL("assets/textures/grass_block_side.png", false);
    /// End of Image Loading

    /// Load Models
    Canis::Model cubeModel = Canis::LoadModel("assets/models/cube.obj");
    Canis::Model grassModel = Canis::LoadModel("assets/models/plants.obj");
    /// END OF LOADING MODEL

    // Load Map into 3d array
    LoadMap("assets/maps/level.map");

    // Loop map and spawn objects
    for (int y = 0; y < map.size(); y++)
    {
        for (int x = 0; x < map[y].size(); x++)
        {
            for (int z = 0; z < map[y][x].size(); z++)
            {
                Canis::Entity entity;
                entity.active = true;

                switch (map[y][x][z])
                {
                case 1: // places a grass block
                    entity.tag = "grass";
                    entity.albedo = &grassTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &cubeModel;
                    entity.shader = &shader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    entity.Update = &Rotate;
                    world.Spawn(entity);
                    break;                
                case 2: // places a grass side block
                    entity.tag = "grass_side";
                    entity.albedo = &grassSideTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &cubeModel;
                    entity.shader = &shader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    world.Spawn(entity);
                    break;
                case 3: // places a flower block
                    entity.tag = "flower";
                    entity.albedo = &blueFlowerTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &grassModel;
                    entity.shader = &grassShader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    entity.Update = &Rotate;
                    world.Spawn(entity);
                    break;
                case 4: // places a glass block (windows)
                    entity.tag = "glass";
                    entity.albedo = &glassTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &cubeModel;
                    entity.shader = &shader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    world.Spawn(entity);
                    break;
                case 5: // places a floor block (house floor)
                    entity.tag = "floor";
                    entity.albedo = &oakPlanksTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &cubeModel;
                    entity.shader = &shader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    world.Spawn(entity);
                    break;
                case 6: // places a fireplace base block
                    entity.tag = "fireplace";
                    entity.albedo = &bricksTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &cubeModel;
                    entity.shader = &shader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    world.Spawn(entity);
                    break;
                case 8: // places a house wall block (oak log)
                    entity.tag = "wall";
                    entity.albedo = &oakLogTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &cubeModel;
                    entity.shader = &shader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    world.Spawn(entity);
                    break;
                case 9: // places a house roof block (cobblestone)
                    entity.tag = "roof";
                    entity.albedo = &cobblestoneTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &cubeModel;
                    entity.shader = &shader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    world.Spawn(entity);
                    break;
                case 10: // places a house foundation block (bricks)
                    entity.tag = "foundation";
                    entity.albedo = &bricksTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &cubeModel;
                    entity.shader = &shader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    world.Spawn(entity);
                    break;
                default:
                    break;
                }
            }
        }
    }

    double deltaTime = 0.0;
    double fps = 0.0;

    // Application loop
    while (inputManager.Update(Canis::GetConfig().width, Canis::GetConfig().heigth))
    {
        deltaTime = frameRateManager.StartFrame();
        Canis::Graphics::ClearBuffer(COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT);

        world.Update(deltaTime);
        world.Draw(deltaTime);

        editor.Draw();

        window.SwapBuffer();

        // EndFrame will pause the app when running faster than frame limit
        fps = frameRateManager.EndFrame();

        //Canis::Log("FPS: " + std::to_string(fps) + " DeltaTime: " + std::to_string(deltaTime));
    }

    return 0;
}

void Rotate(Canis::World &_world, Canis::Entity &_entity, float _deltaTime)
{
    //_entity.transform.rotation.y += _deltaTime;
}

void LoadMap(std::string _path)
{
    // Initialize the map with 5 layers (ground, foundation, floor, walls, roof)
    map = std::vector<std::vector<std::vector<unsigned int>>>(5, 
        std::vector<std::vector<unsigned int>>(15, 
            std::vector<unsigned int>(15, 0)));

    // Fill ground layer with grass blocks
    for (int x = 0; x < 15; x++) {
        for (int z = 0; z < 15; z++) {
            map[0][x][z] = 1; // Grass block
        }
    }

    // House dimensions
    int houseStartX = 5;
    int houseEndX = 10;
    int houseStartZ = 5;
    int houseEndZ = 10;

    // Create foundation (layer 1)
    for (int x = houseStartX; x <= houseEndX; x++) {
        for (int z = houseStartZ; z <= houseEndZ; z++) {
            map[1][x][z] = 10; // Foundation blocks
        }
    }

    // Create floor (layer 2)
    for (int x = houseStartX; x <= houseEndX; x++) {
        for (int z = houseStartZ; z <= houseEndZ; z++) {
            map[2][x][z] = 5; // Floor blocks
        }
    }

    // Create walls (layer 3)
    for (int x = houseStartX; x <= houseEndX; x++) {
        for (int z = houseStartZ; z <= houseEndZ; z++) {
            if (x == houseStartX || x == houseEndX || z == houseStartZ || z == houseEndZ) {
                map[3][x][z] = 8; // Wall blocks
            }
        }
    }

    // Add windows
    map[3][houseStartX][(houseStartZ + houseEndZ) / 2] = 4; // Window on north wall
    map[3][houseEndX][(houseStartZ + houseEndZ) / 2] = 4;   // Window on south wall
    map[3][(houseStartX + houseEndX) / 2][houseStartZ] = 4; // Window on west wall
    map[3][(houseStartX + houseEndX) / 2][houseEndZ] = 4;   // Window on east wall

    // Create roof (layer 4)
    for (int x = houseStartX - 1; x <= houseEndX + 1; x++) {
        for (int z = houseStartZ - 1; z <= houseEndZ + 1; z++) {
            if (x >= houseStartX && x <= houseEndX && z >= houseStartZ && z <= houseEndZ) {
                map[4][x][z] = 9; // Roof blocks
            }
        }
    }

    // Add random grass and flowers around the house
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < 20; i++) {
        int x = rand() % 15;
        int z = rand() % 15;
        
        // Don't place grass/flowers where the house is
        if (x < houseStartX - 1 || x > houseEndX + 1 || z < houseStartZ - 1 || z > houseEndZ + 1) {
            if (rand() % 2 == 0) {
                map[0][x][z] = 2; // Grass side block
            } else {
                map[0][x][z] = 3; // Flower block
            }
        }
    }
}

void SpawnLights(Canis::World &_world)
{
    Canis::DirectionalLight directionalLight;
    _world.SpawnDirectionalLight(directionalLight);

    Canis::PointLight pointLight;
    pointLight.position = vec3(0.0f);
    pointLight.ambient = vec3(0.2f);
    pointLight.diffuse = vec3(0.5f);
    pointLight.specular = vec3(1.0f);
    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;
    pointLight.quadratic = 0.032f;

    _world.SpawnPointLight(pointLight);

    pointLight.position = vec3(0.0f, 0.0f, 1.0f);
    pointLight.ambient = vec3(4.0f, 0.0f, 0.0f);

    _world.SpawnPointLight(pointLight);

    pointLight.position = vec3(-2.0f);
    pointLight.ambient = vec3(0.0f, 4.0f, 0.0f);

    _world.SpawnPointLight(pointLight);

    pointLight.position = vec3(2.0f);
    pointLight.ambient = vec3(0.0f, 0.0f, 4.0f);

    _world.SpawnPointLight(pointLight);
}