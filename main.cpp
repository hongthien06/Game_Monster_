#define SDL_MAIN_HANDLED

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cassert>
struct SDLApplication{
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;


     //For my application to run indefinitely
      bool wRunning = true;

      //A test surface for us to play with
      SDL_Surface* mSurface;


     //Constructor
     SDLApplication(){
       SDL_Init(SDL_INIT_VIDEO);
    mWindow = SDL_CreateWindow("Hello SDL3", 800, 600, SDL_WINDOW_RESIZABLE);
   
    mRenderer = SDL_CreateRenderer(mWindow, nullptr);
     if(mRenderer == nullptr){
        assert(0 && "Not able to create HW accelerated renderer");
     } else {
        SDL_Log("Renderer: %s",SDL_GetRendererName(mRenderer));
     }

    //  mSurface = SDL_LoadBMP("Asset/Flappy_Bird.bmp");
    // if (mSurface == nullptr){
    //   assert(0 && "Improper file path found");
      //}
     }
//     
//     d
//     s
//     da
//     d
//     asd
//     
     
     //Destructor
     ~SDLApplication(){
        SDL_Quit();
     }
     
     void Tick(){
       Input();
       Update();
       Render();
     }

     //Handle input events from I/O or networking devices
        void Input(){

        SDL_Event event;

        const bool* key = SDL_GetKeyboardState(nullptr); 

        while(SDL_PollEvent(&event)){   //SDL_PollEvent dam nhan vai tro chung ve viec kiem tra su kien.VD: Nhan phim, di chuyen chuot, dong cua so... Giong nhu mot ham tra ve true neu co su kien xay ra va luu su kien do vao bien event
            if(event.type == SDL_EVENT_QUIT){
                wRunning = false;
            }
            else if(event.type == SDL_EVENT_KEY_DOWN){
               SDL_Log("Phim da duoc nhan %d",event.key.key);//event.key.keysym.sym se tra ve ma so cua phim duoc nhan xuong
            }         
            else if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
                 if(event.button.button==SDL_BUTTON_LEFT){
                    SDL_Log("Ban da nhan chuot trai %d", event.button.button);
                }
                 if(event.button.button==SDL_BUTTON_RIGHT){
                    SDL_Log("Ban da nhan chuot phai %d", event.button.button);
                }
                 if(event.button.button==SDL_BUTTON_MIDDLE){
                    SDL_Log("Ban da nhan chuot giua %d", event.button.button);
            }
        }

        if(key[SDL_SCANCODE_A]==true){
            SDL_Log("Ban dang nhan phim A");
        }

        float x,y;
        //SDL_MouseButtonFlags mouseState = SDL_GetMouseState(&x,&y);  *Note:Lay vi tri hien tai cua chuot trong cua so
        SDL_MouseButtonFlags mouseState = SDL_GetGlobalMouseState(&x,&y);    //Lay vi tri hien tai cua chuot tren man hinh
        SDL_Log("Vi tri chuot hien tai x: %f, y: %f",x,y);

        // Application/game logic 

        }
    }

    void Update(){

    }
    
    void Render(){
        SDL_SetRenderDrawColor(mRenderer,0x12,0xAB,0xFF,0xFF);
        SDL_RenderClear(mRenderer);

        SDL_SetRenderDrawColor(mRenderer,0xFF,0xFF,0xFF,0xFF);
        SDL_RenderLine(mRenderer,0.0f, 0.0f, 100.0f, 50.0f);

        SDL_FRect rect{
            .x = 100,
            .y = 50,
            .w = 100,
            .h = 100
        };
        
        // SDL_FPoint point{
        //     .x = 100,
        //     .y = 300,
        // };
         
        SDL_RenderRect(mRenderer, &rect);

        SDL_RenderPoint(mRenderer, );

        SDL_RenderPresent(mRenderer);

        

     //   SDL_Surface* windowSurface = SDL_GetWindowSurface(mWindow);
    //    if (nullptr != windowSurface){
     //      SDL_BlitSurface(mSurface, nullptr, windowSurface, nullptr);
     //      SDL_UpdateWindowSurface(mWindow);
     //   }
    }

    //Main application loop
    void MainLoop(){
        while(wRunning){
          Tick();
        }
    }    
};


int main(int argc, char* argv[]){   
     SDLApplication app;
     app.MainLoop();

    return 0;
}


































   
    //*****Danh sách công dụng của các hàm*****
    //SDL_GetKeyboardState: Lấy trạng thái hiện tại của tất cả các phím trên bàn phím. Trả về một con trỏ đến một mảng boolean, trong đó mỗi phần tử đại diện cho trạng thái của một phím cụ thể (được xác định bởi mã quét SDL_Scancode). Nếu phần tử là true, phím tương ứng đang được nhấn; nếu false, phím không được nhấn.
    //SDL_PollEvent: Kiểm tra và xử lý các sự kiện trong hàng đợi sự kiện của SDL. Nếu có sự kiện nào đó, nó sẽ được lấy ra và lưu vào biến event được truyền vào. Hàm này trả về 1 nếu có sự kiện, và 0 nếu không có sự kiện nào.
    //SDL_GetWindowSize: Lấy kích thước hiện tại của cửa sổ SDL được chỉ định. Kích thước được trả về thông qua các tham số con trỏ w và h, đại diện cho chiều rộng và chiều cao của cửa sổ.
    //SDL_WarpMouseInWindow: Di chuyển con trỏ chuột đến một vị trí cụ thể trong cửa sổ SDL được chỉ định. Vị trí được xác định bởi các tham số x và y, đại diện cho tọa độ trong cửa sổ.
    //SDL_GetMouseState: Lấy trạng thái hiện tại của các nút chuột và vị trí của con trỏ chuột trong cửa sổ SDL được chỉ định. Trả về một giá trị bitmask đại diện cho trạng thái của các nút chuột (như nút trái, phải, giữa). Vị trí của con trỏ chuột được trả về thông qua các tham số con trỏ x và y.
    //SDL_GetGlobalMouseState: Lấy trạng thái hiện tại của các nút chuột và vị trí của con trỏ chuột trên màn hình. Tương tự như SDL_GetMouseState.
    
    //*****Danh sách giải thích các code truy xuất*****
    //event.type: Truy cập loại sự kiện (event type) từ cấu trúc
    //event.key.key: Truy cập mã phím (key code) của sự kiện phím từ cấu trúc sự kiện SDL_Event. Mã phím này xác định phím cụ thể đã được nhấn hoặc thả.
    //event.motion.xrel: Truy cập sự thay đổi tương đối về vị trí x của con trỏ chuột trong sự kiện di chuyển chuột (mouse motion event). Giá trị này cho biết con trỏ đã di chuyển bao nhiêu đơn vị theo trục x kể từ lần cập nhật vị trí trước đó.
    //event.motion.yrel: Tương tự như xrel, nhưng cho trục y. Giá trị này cho biết con trỏ đã di chuyển bao nhiêu đơn vị theo trục y kể từ lần cập nhật vị trí trước đó.
    //event.button.button: Truy cập mã nút chuột (mouse button code) của sự kiện nút chuột từ cấu trúc sự kiện SDL_Event. Mã này xác định nút chuột cụ thể đã được nhấn hoặc thả (ví dụ: nút trái, phải, giữa).
    //key[SDL_SCANCODE_A]: Truy cập trạng thái của phím 'A' từ mảng trạng thái bàn phím trả về bởi SDL_GetKeyboardState. Nếu giá trị là true, phím 'A' đang được nhấn; nếu false, phím không được nhấn.
    //mouseState: Biến lưu trữ trạng thái hiện tại của các nút chuột, được trả về bởi hàm SDL_GetMouseState hoặc SDL_GetGlobalMouseState. Giá trị này là một bitmask, trong đó mỗi bit đại diện cho trạng thái của một nút chuột cụ thể (như nút trái, phải, giữa).
    //x, y: Tham số con trỏ được sử dụng để lưu trữ vị trí hiện tại của con trỏ chuột. Các giá trị này được cập nhật bởi hàm SDL_GetMouseState hoặc SDL_GetGlobalMouseState để phản ánh tọa độ x và y của con trỏ trên cửa sổ hoặc màn hình.            
    //SDL_BUTTON_LEFT, SDL_BUTTON_RIGHT, SDL_BUTTON_MIDDLE: Các hằng số định nghĩa mã nút chuột cho nút trái, phải và giữa. Chúng được sử dụng để so sánh với giá trị event.button.button để xác định nút chuột nào đã được nhấn trong sự kiện nút chuột.
    //SDL_SCANCODE_A: Hằng số định nghĩa mã quét (scancode) cho phím 'A' trên bàn phím. Nó được sử dụng để truy cập trạng thái của phím 'A' trong mảng trả về bởi SDL_GetKeyboardState. Nếu key[SDL_SCANCODE_A] là true, điều đó có nghĩa là phím 'A' đang được nhấn.   
    
