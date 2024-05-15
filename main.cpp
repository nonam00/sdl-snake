#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>
#include <deque>
#include <ctime>

#define WINDOW_SIZE_X 1000
#define WINDOW_SIZE_Y 1000
#define CUBE_SIZE 20

// create apples in the mapp
void set_apples(std::vector<SDL_Rect>& apples)
{
  apples.clear();
  int min = CUBE_SIZE; 
  int max_x = (WINDOW_SIZE_X - CUBE_SIZE) / (CUBE_SIZE * 2);
  int max_y = (WINDOW_SIZE_Y - CUBE_SIZE) / (CUBE_SIZE * 2);
  for(int i = 0; i < 10; i++)
  {
    apples.emplace_back(
        rand() % (max_x) * CUBE_SIZE * 2 + min,
        rand() % (max_y) * CUBE_SIZE * 2 + min,
        CUBE_SIZE, CUBE_SIZE);
  }
}

void game_reset(SDL_Rect& head, std::vector<SDL_Rect>& apples, int& size)
{
  size = 1;
  head.x = WINDOW_SIZE_X / 2;
  head.y = WINDOW_SIZE_Y / 2;
  set_apples(apples);
}

int main()
{
  srand(time(NULL));
  SDL_Init(SDL_INIT_EVERYTHING);
  auto window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0);
  auto renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Event e;

  enum Direction
  {
    DOWN,
    LEFT,
    RIGHT,
    UP
  };
  
  // visual display of the playground
  SDL_Rect playground {CUBE_SIZE, CUBE_SIZE, WINDOW_SIZE_X - CUBE_SIZE * 2, WINDOW_SIZE_Y - CUBE_SIZE * 2};
  
  // snake head position
  SDL_Rect head {WINDOW_SIZE_X / 2, WINDOW_SIZE_Y / 2, CUBE_SIZE, CUBE_SIZE};
  
  //snake body container
  std::deque<SDL_Rect> rq;
  int size = 1;

  // apple container
  std::vector<SDL_Rect> apples;
  
  // create apples in the map
  set_apples(apples);

  bool running = true;
  int dir = 0;
  while(running)
  {
    // check input 
    while(SDL_PollEvent(&e))
    {
      if(e.type == SDL_QUIT) { running = false; }
      else if(e.type == SDL_KEYDOWN) {
        if(e.key.keysym.sym == SDLK_DOWN && dir != UP)    { dir = DOWN; }
        if(e.key.keysym.sym == SDLK_UP && dir != DOWN)    { dir = UP; }
        if(e.key.keysym.sym == SDLK_LEFT && dir != RIGHT) { dir = LEFT; }
        if(e.key.keysym.sym == SDLK_RIGHT && dir != LEFT) { dir = RIGHT; }
      }
    }

    // move
    switch(dir)
    {
      case DOWN:
        head.y += CUBE_SIZE; break;
      case UP:
        head.y -= CUBE_SIZE; break;
      case LEFT:
        head.x -= CUBE_SIZE; break;
      case RIGHT:
        head.x += CUBE_SIZE; break;
    }
    // collision detection with apple
    std::for_each(apples.begin(), apples.end(), [&](auto& apple)
    {
      if(head.x == apple.x && head.y == apple.y)
      {
        size += CUBE_SIZE;
        apple.x = -CUBE_SIZE;
        apple.y = -CUBE_SIZE;
      }
    });
    
    // collision detection with snake body
    std::for_each(rq.begin(), rq.end(), [&](auto& snake_segment)
    {
      if(head.x == snake_segment.x && head.y == snake_segment.y)
      {
        game_reset(head, apples, size);
      }
    });
    
    // collision detection with playground borders
    if(head.x < CUBE_SIZE ||
      head.y < CUBE_SIZE ||
      head.x > WINDOW_SIZE_X - CUBE_SIZE * 2 ||
      head.y > WINDOW_SIZE_Y - CUBE_SIZE * 2)
    {
      game_reset(head, apples, size);
    }

    // add newest head ot the snake
    rq.push_front(head);

    // removing segments that do not match the size
    while(rq.size() > size)
    {
      rq.pop_back();
    }

    // clear window
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderClear(renderer);
   
    // draw playground
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &playground);
 
    // draw body
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    std::for_each(rq.begin(), rq.end(), [&](auto& snake_segment)
    {
      SDL_RenderFillRect(renderer, &snake_segment);
    });
    SDL_RenderFillRect(renderer, &head);
    
    //draw apples
    SDL_SetRenderDrawColor(renderer,182,80,212,255);
    std::for_each(apples.begin(), apples.end(), [&](auto& apple)
    {
      SDL_RenderFillRect(renderer, &apple);
    });

    // display
    SDL_RenderPresent(renderer);
    SDL_Delay(40);
  }

  return 0;
}
