#ifndef CONSTANTS_H
#define CONSTANTS_H

// Kích thước của người chơi
const int spriteHeight = 35;
const int spriteWidth = 35;

// Kích thước của một ô trong bàn cờ
const int TILE_SIZE = 89;

// Số lượng ô trên bàn cờ
const int NUM_TILES = 32;
const int numTilesPerSide = 8;

const int OFFSET = 50; // Đặt thêm khoảng cách giữa các ô và viền bàn cờ (nếu cần)

// Tốc độ di chuyển của người chơi
const float PLAYER_SPEED = 200.0f;

// Kích thước của cửa sổ game
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

// Tài sản khởi đầu của người chơi
const int STARTING_MONEY = 1500;

// Đường dẫn tới hình ảnh ngôi nhà
const std::string HOUSE_TEXTURE_PATH = "resized_house_image.png";

// Phí mua bãi biển
const int BEACH_COST = 200;

// Tối đa nhà có thể mua trên 1 ô đất
const int MAX_HOUSE = 4;


#endif

