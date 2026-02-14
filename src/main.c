#include <float.h>
#include <math.h>
#include <raylib.h>
#include <raymath.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SPHERE_COUNT 5

typedef struct {
  Vector3 position;
  float radius;
  Color color;
} Sphere;

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raytracer");

  Color pixels[SCREEN_HEIGHT][SCREEN_WIDTH];
  Sphere spheres[SPHERE_COUNT];
  Sphere sphere;

  spheres[0] = (Sphere){.position = {3, 0, 10}, .radius = 2.f, .color = RED};
  spheres[1] = (Sphere){.position = {5, -5, 14}, .radius = 3.f, .color = BLUE};
  spheres[2] =
      (Sphere){.position = {0, 2, 20}, .radius = 5.f, .color = RAYWHITE};
  spheres[3] =
      (Sphere){.position = {-2, -1, 8}, .radius = 1.f, .color = PURPLE};
  spheres[4] =
      (Sphere){.position = {1, 2, 5}, .radius = 0.5f, .color = DARKBLUE};

  Camera camera = {.position = {0, 0, 0},
                   .target = {0, 0, 10},
                   .up = {0, 1, 0},
                   .fovy = 60,
                   .projection = CAMERA_PERSPECTIVE};

  Image image = {.data = pixels,
                 .width = SCREEN_WIDTH,
                 .height = SCREEN_HEIGHT,
                 .mipmaps = 1,
                 .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};

  Texture2D texture = LoadTextureFromImage(image);
  Vector3 light_dir = Vector3Normalize((Vector3){1, 1, -0.5});

  while (!WindowShouldClose()) {
    BeginDrawing();
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      for (int y = 0; y < SCREEN_HEIGHT; y++) {
        Ray ray = GetScreenToWorldRay((Vector2){x, y}, camera);
        RayCollision nearest_collision;
        Sphere nearest_sphere;
        nearest_collision.hit = false;
        nearest_collision.distance = FLT_MAX;

        for (int i = 0; i < SPHERE_COUNT; i++) {
          sphere = spheres[i];
          RayCollision collision =
              GetRayCollisionSphere(ray, sphere.position, sphere.radius);

          if (!collision.hit) {
            continue;
          }

          if (collision.distance < nearest_collision.distance) {
            nearest_collision = collision;
            nearest_sphere = sphere;
          }
        }

        if (!nearest_collision.hit) {
          pixels[y][x] = BLACK;
          continue;
        }

        float diffuse =
            fmaxf(Vector3DotProduct(nearest_collision.normal, light_dir), 0.0);

        pixels[y][x] = (Color){.r = nearest_sphere.color.r * diffuse,
                               .g = nearest_sphere.color.g * diffuse,
                               .b = nearest_sphere.color.b * diffuse,
                               .a = nearest_sphere.color.a};
      }
    }
    UpdateTexture(texture, pixels);
    DrawTexture(texture, 0, 0, WHITE);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
