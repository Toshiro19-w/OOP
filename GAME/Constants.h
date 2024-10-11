#ifndef CONSTANTS_H
#define CONSTANTS_H

// Kích thước của người chơi
const int spriteHeight = 35;
const int spriteWidth = 35;

// Kích thước của một ô trong bàn cờ
const int TILE_SIZE = 70;

// Số lượng ô trên bàn cờ
const int NUM_TILES = 40;

const int numTilesPerSide = 11;

// Tốc độ di chuyển của người chơi
const float PLAYER_SPEED = 200.0f;

// Kích thước của cửa sổ game
const int SCREEN_WIDTH = 770;
const int SCREEN_HEIGHT = 770;

// Tài sản khởi đầu của người chơi
const int STARTING_MONEY = 1500;

// Đường dẫn tới hình ảnh ngôi nhà
const std::string HOUSE_TEXTURE_PATH = "resized_house_image.png";

// Số lượt ở trong tù
const int MAX_TURNS_IN_JAIL = 3;

// Phí ra tù
const int JAIL_FEE = 200;

#endif

