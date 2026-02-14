#include <float.h>
#include <math.h>
#include <raylib.h>
#include <raymath.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SPHERE_COUNT 5

typedef struct {
  Color color;
  float specular;
  float shininess;
} SphereMaterial;

typedef struct {
  Vector3 position;
  float radius;
  SphereMaterial material;
} Sphere;

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raytracer");

  SphereMaterial red = {.color=RED, .specular=1.f, .shininess=256};

  Color pixels[SCREEN_HEIGHT][SCREEN_WIDTH];
  Sphere spheres[SPHERE_COUNT];
  Sphere sphere;

  spheres[0] = (Sphere){.position = {3, 0, 10}, .radius = 2.f, .material = red};
  spheres[1] = (Sphere){.position = {5, -5, 14}, .radius = 3.f, .material = red};
  spheres[2] =
      (Sphere){.position = {0, 2, 20}, .radius = 5.f, .material = red};
  spheres[3] =
      (Sphere){.position = {-2, -1, 8}, .radius = 1.f,.material = red};
  spheres[4] =
      (Sphere){.position = {1, 2, 5}, .radius = 0.5f, .material = red};

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
  Vector3 light_dir = Vector3Normalize((Vector3){0.4, 1, -0.8});

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

        SphereMaterial material = nearest_sphere.material;
        float diffuse =
            fmaxf(Vector3DotProduct(nearest_collision.normal, light_dir), 0.0);
        
        Vector3 view_dir = Vector3Normalize(Vector3Subtract(camera.position, nearest_collision.point));
        Vector3 reflect_dir = Vector3Reflect(Vector3Negate(light_dir), nearest_collision.normal);
        
        float specular = pow(fmaxf(Vector3DotProduct(view_dir, reflect_dir), 0.0), material.shininess) *
        material.specular;

        float result = Clamp(diffuse + specular, 0, 1);


        pixels[y][x] = (Color){.r = material.color.r * (result),
                               .g = material.color.g * (result),
                               .b = material.color.b * (result),
                               .a = material.color.a};
      }
    }
    UpdateTexture(texture, pixels);
    DrawTexture(texture, 0, 0, WHITE);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
