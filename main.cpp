#include <stdio.h>

#include "raylib.h"
#include <cstdint>
#include <cstdlib> // for malloc and free
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define PIXEL_SIZE 10

#define MEMORY_SIZE 4096
#define STACK_SIZE 16
#define PROGRAM_OFFSET 0x200
#define VX_SIZE 16
#define FONT_SIZE 80
#define FONT_START_ADDRES 0x0

int font[FONT_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void loadFont(uint8_t *&memory) {
  for (int i = 0; i < FONT_SIZE; i++) {
    *(memory + i) = static_cast<char>(font[i]);
  }
}

void loadProgram(const char *path, uint8_t *buffer) {
  // Open file
  std::ifstream infile(path, std::ios_base::binary);

  // Check if file opened successfully
  if (!infile) {
    std::cerr << "Error opening file: " << path << std::endl;
    return;
  }

  // Get length of file
  infile.seekg(0, std::ios::end);
  size_t length = infile.tellg();
  infile.seekg(0, std::ios::beg);

  // Read file
  infile.read((char *)buffer, length);
}

void drawDisplay(const bool (&display)[DISPLAY_HEIGHT][DISPLAY_WIDTH]) {
  // Loop through the CHIP-8 display array and draw pixels
  for (int y = 0; y < DISPLAY_HEIGHT; y++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      if (display[y][x]) {
        // Draw a rectangle for each "on" pixel
        DrawRectangle(x * PIXEL_SIZE, y * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE,
                      WHITE);
      }
    }
  }
}

void clearDisplay(bool (&display)[DISPLAY_HEIGHT][DISPLAY_WIDTH]) {
  // Clear the CHIP-8 display array
  for (int y = 0; y < DISPLAY_HEIGHT; y++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      display[y][x] = false;
    }
  }
}

void stepProgram(uint8_t *&memory, uint8_t *&program, uint8_t *&Vx, uint16_t &I,
                 uint16_t *&stack, uint8_t &DT, uint8_t &ST,
                 bool (&display)[32][64], bool (&keyPresses)[16]) {
  uint8_t left_op = *program;
  uint8_t right_op = *(program + 1);
  uint8_t first = (left_op & 0xF0) >> 4;
  uint8_t second = left_op & 0x0F;
  uint8_t third = (right_op & 0xF0) >> 4;
  uint8_t fourth = right_op & 0x0F; // No need to shift for the low nibble
  uint16_t opcode = (left_op << 8) | (right_op);
  uint16_t last_3 = opcode & 0x0FFF;

  uint8_t vx = *(Vx + second);
  uint8_t vy = *(Vx + third);

  if (opcode == 0x00E0) { // Clear display
    clearDisplay(display);
  } else if (opcode == 0x00EE) {
    stack--;
    program = memory + *stack;
    *stack = 0;
  } else if (first == 0x1) { // Jump to address NNN
    program = memory + last_3;
    return;
  } else if (first == 0x2) {
    *stack = program - memory;
    stack++;
    program = memory + last_3;
    return;
  } else if (first == 0x3) {
    if (*(Vx + second) == right_op) {
      program += 2;
    }
  } else if (first == 0x4) {
    if (*(Vx + second) != right_op) {
      program += 2;
    }
  } else if (first == 0x5) {
    if (*(Vx + second) == *(Vx + third)) {
      program += 2;
    }
  } else if (first == 0x6) { // Set VX to NN
    *(Vx + second) = right_op;
  } else if (first == 0x7) { // Set VX to NN
    *(Vx + second) += right_op;
  } else if (first == 0x8) {
    uint16_t res;
    uint8_t val1, val2;

    switch (fourth) {
    case 0x0:
      *(Vx + second) = vy;
      break;
    case 0x1:
      *(Vx + second) = vx | vy;
      break;
    case 0x2:
      *(Vx + second) = vx & vy;
      break;
    case 0x3:
      *(Vx + second) = vx ^ vy;
      break;
    case 0x4:
      res = ((uint16_t)vx) + ((uint16_t)vy);

      if (res > 255) {
        *(Vx + 0xF) = 1;
        res &= 0xFF;
      } else {
        *(Vx + 0xF) = 0;
      }

      *(Vx + second) = (uint8_t)res;
      break;
    case 0x5:
      *(Vx + 0xF) = (vx < vy) ? 0 : 1;
      *(Vx + second) = vx - vy;
      break;
    case 0x7:
      // Set VF = NOT borrow (1 if VY >= VX, 0 if VY < VX)
      *(Vx + 0xF) = (vy >= vx) ? 1 : 0;
      *(Vx + second) = vy - vx;
      break;
    case 0x6:
      *(Vx + 0xF) = (vx & 0b1) ? 1 : 0;
      *(Vx + second) = vx >> 1;
      break;
    case 0xE:
      *(Vx + 0xF) = (vx >> 15 & 0b1) ? 1 : 0;
      *(Vx + second) = vx << 1;
      break;
    default:
      break;
    }
  } else if (first == 0x9) {
    if (vx != vy) {
      program += 2;
    }
  } else if (first == 0xA) { // Set I to NNN
    I = last_3;
  } else if (first == 0xC) {
    *(Vx + second) = (rand() % 255) & right_op;
  } else if (first == 0xD) {      // Draw sprite (Dxyn)
    uint8_t n = fourth;           // Number of rows (N)
    uint8_t *sprite = memory + I; // Sprite data starts at I
    bool collision = false;

    for (int row = 0; row < n; row++) {
      uint8_t sprite_byte = *(sprite + row);

      for (int col = 0; col < 8; col++) {
        // Get the pixel value of the sprite (each bit in the sprite byte)
        bool sprite_pixel = (sprite_byte >> (7 - col)) & 0x1;
        int x = (vx + col) % 64; // Wrap around X (64 wide)
        int y = (vy + row) % 32; // Wrap around Y (32 high)

        // XOR the sprite pixel with the current screen pixel
        if (sprite_pixel && display[y][x]) {
          collision = true; // A collision happened
        }
        display[y][x] ^= sprite_pixel; // XOR the sprite onto the display
      }
    }
    *(Vx + 0xF) = collision ? 1 : 0; // Set VF to 1 if there was a collision
  } else if (first == 0xE) {
    if (right_op == 0x9E) {
      if (keyPresses[vx]) {
        program += 2;
      }
    } else if (right_op == 0xA1) {
      if (!keyPresses[vx]) {
        program += 2;
      }
    } else {
      printf("E command Unknown right_op: %02x\n", right_op);
    }
  } else if (first == 0xF) {
    uint16_t val;

    switch (right_op) {
    case 0x07:
      *(Vx + second) = DT;
      break;
    case 0x0A:
      static int found = -1;
      static bool released = false;

      for (int i = 0; i < 16; i++) {
        if (keyPresses[i]) {
          *(Vx + second) = i;
          found = i;
        } else {
          if (found != -1 && !released && found == i) {
            released = true;
          }
        }
      }
      if (found == -1 || !released) {
        program -= 2;
      } else {
        found = -1;
        released = false;
      }
      break;
    case 0x15:
      DT = vx;
      break;
    case 0x18:
      ST = vx;
      break;
    case 0x29:
      I = FONT_START_ADDRES + (5 * vx);
      break;
    case 0x1E:
      I += (uint8_t) * (Vx + second);
      break;
    case 0x33:
      val = (uint8_t) * (Vx + second);
      *(memory + I) = (char)(val / 100);
      *(memory + I + 1) = (char)(val % 100 / 10);
      *(memory + I + 2) = (char)(val % 10);
      break;
    case 0x55:
      for (int i = 0; i <= second; i++) {
        *(memory + I + i) = *(Vx + i);
      }
      break;
    case 0x65:
      for (int i = 0; i <= second; i++) {
        *(Vx + i) = *(memory + I + i);
      }
      break;
    default:
      printf("F command Unknown right_op: %02x\n", right_op);
    }
  } else if (opcode == 0) {
    exit(0);
  } else {
    printf("GENERAL Unknown opcode: %04x\n", opcode);
  }

  program += 2; // Move to the next opcode
}

void soundControl(bool, Sound);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
    return 1;
  }

  // Init raylib stuff
  InitWindow(DISPLAY_WIDTH * PIXEL_SIZE, DISPLAY_HEIGHT * PIXEL_SIZE,
             "CHIP-8 Emulator");
  InitAudioDevice();
  Sound beep = LoadSound("assets/beep.wav");
  SetTargetFPS(60);

  // Initialize CHIP-8 components
  bool display[DISPLAY_HEIGHT][DISPLAY_WIDTH] = {false};
  uint8_t *memory = (uint8_t *)calloc(MEMORY_SIZE, sizeof(uint8_t));
  uint8_t *Vx = (uint8_t *)calloc(VX_SIZE, sizeof(uint8_t));
  uint16_t *stack = (uint16_t *)calloc(STACK_SIZE, sizeof(uint16_t));
  uint16_t I = 0;
  uint8_t DT = 0;
  uint8_t ST = 0;

  uint8_t *program = memory + PROGRAM_OFFSET;

  // Load program and font
  loadProgram(argv[1], program);
  loadFont(memory);

  // Initialize timing variables
  double lastTimerUpdate = GetTime();
  const double timerInterval = 1.0 / 60.0; // Timers update at 60Hz
  const int cyclesPerFrame = 10;

  // Key states
  bool keyPresses[16] = {};

  while (!WindowShouldClose()) {

    // Execute multiple CPU cycles per frame
    for (int i = 0; i < cyclesPerFrame; i++) {
      // Update keyPresses
      keyPresses[0x0] = IsKeyDown(KEY_X);
      keyPresses[0x1] = IsKeyDown(KEY_ONE);
      keyPresses[0x2] = IsKeyDown(KEY_TWO);
      keyPresses[0x3] = IsKeyDown(KEY_THREE);
      keyPresses[0x4] = IsKeyDown(KEY_Q);
      keyPresses[0x5] = IsKeyDown(KEY_W);
      keyPresses[0x6] = IsKeyDown(KEY_E);
      keyPresses[0x7] = IsKeyDown(KEY_A);
      keyPresses[0x8] = IsKeyDown(KEY_S);
      keyPresses[0x9] = IsKeyDown(KEY_D);
      keyPresses[0xA] = IsKeyDown(KEY_Z);
      keyPresses[0xB] = IsKeyDown(KEY_C);
      keyPresses[0xC] = IsKeyDown(KEY_FOUR);
      keyPresses[0xD] = IsKeyDown(KEY_R);
      keyPresses[0xE] = IsKeyDown(KEY_F);
      keyPresses[0xF] = IsKeyDown(KEY_V);
      stepProgram(memory, program, Vx, I, stack, DT, ST, display, keyPresses);
    }

    // Update timers at 60Hz
    double currentTime = GetTime();
    if ((currentTime - lastTimerUpdate) >= timerInterval) {
      if (DT > 0)
        DT--;
      if (ST > 0)
        ST--;
      lastTimerUpdate += timerInterval;
    }

    // Draw the display
    BeginDrawing();
    ClearBackground(BLACK);
    drawDisplay(display);
    EndDrawing();

    // Handle sound
    bool shouldBeep = ST > 0;
    if (shouldBeep && !IsSoundPlaying(beep)) {
      PlaySound(beep);
    } else if (!shouldBeep && IsSoundPlaying(beep)) {
      StopSound(beep);
    }
  }

  // Cleanup
  UnloadSound(beep);
  CloseAudioDevice();
  CloseWindow();

  free(memory);
  free(Vx);
  free(stack);

  return 0;
}
