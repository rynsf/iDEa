#include <stdio.h>
#include <SDL2/SDL.h>

#define SIZEX 1400 //do i have to change the resolution
#define SIZEY 800
#define NOT_GATE 0
#define AND_GATE 1
#define OR_GATE 2
#define LOW 0
#define HIGH 1

SDL_Window* win;
SDL_Renderer* rend;
SDL_Texture* and_texture;
SDL_Texture* or_texture;
SDL_Texture* not_texture;
SDL_Texture* buttonoff_texture;
SDL_Texture* buttonon_texture;
SDL_Texture* lightoff_texture;
SDL_Texture* lighton_texture;

typedef struct{
    unsigned int* x;
    unsigned int* y;
    unsigned int z;
    unsigned int type;
    SDL_Rect pos;
} gate;

typedef struct{
    unsigned int state;
    SDL_Rect pos;
} button;

typedef struct{
    unsigned int* state;
    SDL_Rect pos;
} light;

gate g[10];
int gnum;

button b[10];
int bnum;

light l[10];
int lnum;

int Eval(gate* g);
int Free();
SDL_Texture* LoadTextureBMP(const char* file);

int Init()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Failed to Init SDL: %s", SDL_GetError());
        goto error;
    }

    win = SDL_CreateWindow("iDEa", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SIZEX, SIZEY, 0);
    if(win == NULL)
    {
        printf("Failed to create SDL Window: %s", SDL_GetError());
        goto error;
    }

    rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if(rend == NULL)
    {
        printf("Failed to create SDL Renderer: %s", SDL_GetError());
        goto error;
    }

    and_texture = LoadTextureBMP("./assets/and.bmp");
    if(and_texture == NULL)
        goto error;

    or_texture = LoadTextureBMP("./assets/or.bmp");
    if(or_texture == NULL)
        goto error;

    not_texture = LoadTextureBMP("./assets/not.bmp");
    if(not_texture == NULL)
        goto error;

    buttonoff_texture = LoadTextureBMP("./assets/buttonoff.bmp");
    if(buttonoff_texture == NULL)
        goto error;

    buttonon_texture = LoadTextureBMP("./assets/buttonon.bmp");
    if(buttonon_texture == NULL)
        goto error;

    lightoff_texture = LoadTextureBMP("./assets/lightoff.bmp");
    if(lightoff_texture == NULL)
        goto error;

    lighton_texture = LoadTextureBMP("./assets/lighton.bmp");
    if(lighton_texture == NULL)
        goto error;

    return 0;

    error:
        Free();
        return -1;
}

SDL_Texture* LoadTextureBMP(const char* file)
{
    SDL_Surface* bmpSurface = SDL_LoadBMP(file);
    if(bmpSurface == NULL)
    {
        printf("Failed to Load media: %s", SDL_GetError());
        return NULL;
    }
    SDL_Texture* bmpTexture = SDL_CreateTextureFromSurface(rend, bmpSurface);
    SDL_FreeSurface(bmpSurface);
    if(bmpTexture == NULL)
    {
        printf("Failed to Create Texture From Surface: %s", SDL_GetError());
        return NULL;
    }
    return bmpTexture;
}

int Free()
{
    if(win)
        SDL_DestroyWindow(win);
    if(rend)
        SDL_DestroyRenderer(rend);
    if(and_texture)
        SDL_DestroyTexture(and_texture);
    if(or_texture)
        SDL_DestroyTexture(or_texture);
    if(not_texture)
        SDL_DestroyTexture(not_texture);
    if(buttonoff_texture)
        SDL_DestroyTexture(buttonoff_texture);
    if(buttonon_texture)
        SDL_DestroyTexture(buttonon_texture);
    if(lightoff_texture)
        SDL_DestroyTexture(lightoff_texture);
    if(lighton_texture)
        SDL_DestroyTexture(lighton_texture);
    SDL_Quit();
    return 0;
}

int RenderWire()
{
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    for(int n = 0; n < bnum; n++)
    {
        for(int m = 0; m < gnum; m++)
        {
            if(g[m].type == NOT_GATE)
            {
                if(&b[n].state == g[m].x)
                {
                    SDL_RenderDrawLine(rend, b[n].pos.x+100, b[n].pos.y+50, g[m].pos.x, g[m].pos.y+50);
                }
            }
            else
            {
                if(&b[n].state == g[m].x)
                {
                    SDL_RenderDrawLine(rend, b[n].pos.x+100, b[n].pos.y+50, g[m].pos.x, g[m].pos.y+30);
                }
                if(&b[n].state == g[m].y)
                {
                    SDL_RenderDrawLine(rend, b[n].pos.x+100, b[n].pos.y+50, g[m].pos.x, g[m].pos.y+70);
                }
            }
        }

        for(int o = 0; o < lnum; o++)
        {
            if(&b[n].state == l[o].state)
            {
                SDL_RenderDrawLine(rend, b[n].pos.x+100, b[n].pos.y+50, l[o].pos.x, l[o].pos.y+50);
            }
        }
    }

    for(int n = 0; n < gnum; n++)
    {
        for(int m = 0; m < gnum; m++)
        {
            if(g[m].type == NOT_GATE)
            {
                if(&g[n].z == g[m].x)
                {
                    SDL_RenderDrawLine(rend, g[n].pos.x+100, g[n].pos.y+50, g[m].pos.x, g[m].pos.y+50);
                }
            }
            else
            {
                if(&g[n].z == g[m].x)
                {
                    SDL_RenderDrawLine(rend, g[n].pos.x+100, g[n].pos.y+50, g[m].pos.x, g[m].pos.y+30);
                }
                if(&g[n].z == g[m].y)
                {
                    SDL_RenderDrawLine(rend, g[n].pos.x+100, g[n].pos.y+50, g[m].pos.x, g[m].pos.y+70);
                }
            }
        }

        for(int o = 0; o < lnum; o++)
        {
            if(&g[n].z == l[o].state)
            {
                SDL_RenderDrawLine(rend, g[n].pos.x+100, g[n].pos.y+50, l[o].pos.x, l[o].pos.y+50);
            }
        }
    }
    return 0;
}

int Render()
{
    SDL_SetRenderDrawColor(rend, 215, 215, 215, 255);
    SDL_RenderClear(rend);

    for(int i = 0; i < bnum; i++){
        if(b[i].state == LOW){
            SDL_RenderCopy(rend, buttonoff_texture, NULL, &b[i].pos);
        } else {
            SDL_RenderCopy(rend, buttonon_texture, NULL, &b[i].pos);
        }
    }

    for(int i = 0; i < gnum; i++){
        switch(g[i].type) {
            case NOT_GATE:
                SDL_RenderCopy(rend, not_texture, NULL, &g[i].pos);
                break;

            case AND_GATE:
                SDL_RenderCopy(rend, and_texture, NULL, &g[i].pos);
                break;

            case OR_GATE:
                SDL_RenderCopy(rend, or_texture, NULL, &g[i].pos);
                break;

            default: {}
        }
    }

    for(int i = 0; i < lnum; i++){
        if(*l[i].state == LOW){
            SDL_RenderCopy(rend, lightoff_texture, NULL, &l[i].pos);
        } else {
            SDL_RenderCopy(rend, lighton_texture, NULL, &l[i].pos);
        }
    }

    RenderWire();
    SDL_RenderPresent(rend);
    return 0;
}

int CreateButton(button* btn, unsigned int state, int x, int y, int w, int h)
{
    btn -> state = state;
    btn -> pos.x = x;
    btn -> pos.y = y;
    btn -> pos.w = w;
    btn -> pos.h = h;
    return 0;
}

int CreateLight(light* lgt, unsigned int* state, int x, int y, int w, int h)
{
    lgt -> state = state;
    lgt -> pos.x = x;
    lgt -> pos.y = y;
    lgt -> pos.w = w;
    lgt -> pos.h = h;
    return 0;
}

int CreateANDGate(gate* g, unsigned int* x, unsigned int* y, int X, int Y, int w, int h)
{
    g -> pos.x = X;
    g -> pos.y = Y;
    g -> pos.w = w;
    g -> pos.h = h;
    g -> type = AND_GATE;
    g -> x = x;
    g -> y = y;
    Eval(g);
    return 0;
}

int CreateORGate(gate* g, unsigned int* x, unsigned int* y, int X, int Y, int w, int h)
{
    g -> pos.x = X;
    g -> pos.y = Y;
    g -> pos.w = w;
    g -> pos.h = h;
    g -> type = OR_GATE;
    g -> x = x;
    g -> y = y;
    Eval(g);
    return 0;
}

int CreateNOTGate(gate* g, unsigned int* x, int X, int Y, int w, int h)
{
    g -> pos.x = X;
    g -> pos.y = Y;
    g -> pos.w = w;
    g -> pos.h = h;
    g -> type = NOT_GATE;
    g -> x = x;
    g -> y = x;
    Eval(g);
    return 0;
}

int Eval(gate* g)
{
    if(g -> x == NULL || g -> y == NULL)
    {
        return 0;
    }

    switch(g -> type)
    {
        case NOT_GATE:
            g -> z = !(*g -> x);
            break;

        case AND_GATE:
            g -> z = (*g -> x) & (*g -> y);
            break;

        case OR_GATE:
            g -> z = (*g -> x) | (*g -> y);
            break;

        default: {}
    }
    return 0;
}

int OnClickedButton(int i)
{
    b[i].state = b[i].state == LOW ? HIGH : LOW;
    return 0;
}

int OnClick(int x, int y)
{
    for(int i = 0; i < bnum; i++)
    {
        if(x >= b[i].pos.x && y >= b[i].pos.y && x <= b[i].pos.x+b[i].pos.w && y <= b[i].pos.y+b[i].pos.h)
        {
            OnClickedButton(i);
            return 0;
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    if(Init() != 0)
    {
        return -1;
    }

    // making dummy circuit for now
    lnum = 0;
    bnum = 0;
    gnum = 0;

    /*
    CreateButton(&b[bnum++], LOW, 50, 150, 100, 100);
    CreateButton(&b[bnum++], LOW, 50, 250, 100, 100);
    CreateButton(&b[bnum++], LOW, 50, 350, 100, 100);
    CreateButton(&b[bnum++], LOW, 50, 450, 100, 100);

    CreateANDGate(&g[gnum++], &b[0].state, &b[1].state, 200, 200, 100, 100);
    CreateANDGate(&g[gnum++], &b[2].state, &b[3].state, 200, 400, 100, 100);
    CreateORGate(&g[gnum++], &g[0].z, &g[1].z, 400, 300, 100, 100);

    CreateLight(&l[lnum++], &g[2].z, 600, 300, 100, 100);
    */

    /*//Xor with and or not
    CreateButton(&b[bnum++], LOW, 50, 100, 100, 100);
    CreateButton(&b[bnum++], LOW, 50, 500, 100, 100);

    CreateNOTGate(&g[gnum++], &b[0].state, 250, 100, 100, 100);
    CreateNOTGate(&g[gnum++], &b[1].state, 250, 500, 100, 100);
    CreateANDGate(&g[gnum++], &g[0].z, &b[1].state, 500, 200, 100, 100);
    CreateANDGate(&g[gnum++], &b[0].state, &g[1].z, 500, 400, 100, 100);
    CreateORGate(&g[gnum++], &g[2].z, &g[3].z, 650, 300, 100, 100);

    CreateLight(&l[lnum++], &g[4].z, 800, 300, 100, 100);
    */

    //XOR with nand
    CreateButton(&b[bnum++], LOW, 50, 100, 100, 100);
    CreateButton(&b[bnum++], LOW, 50, 300, 100, 100);

    CreateORGate(&g[gnum++], &b[0].state, &b[1].state,  250, 100, 100, 100);
    CreateANDGate(&g[gnum++], &b[0].state, &b[1].state,  250, 300, 100, 100);
    CreateNOTGate(&g[gnum++], &g[1].z, 450, 300, 100, 100);
    CreateANDGate(&g[gnum++], &g[0].z, &g[2].z, 650, 200, 100, 100);

    CreateLight(&l[lnum++], &g[3].z, 850, 200, 100, 100);


    /*
    //SR latch with NOR
    CreateButton(&b[bnum++], LOW, 50, 100, 100, 100);
    CreateButton(&b[bnum++], LOW, 50, 300, 100, 100);

    CreateORGate(&g[gnum++], &b[0].state, NULL,  250, 100, 100, 100);
    CreateNOTGate(&g[gnum++], &g[0].z, 450, 100, 100, 100);
    CreateORGate(&g[gnum++], &g[1].z, &b[1].state, 250, 300, 100, 100);
    CreateNOTGate(&g[gnum++], &g[2].z, 450, 300, 100, 100);
    g[0].y = &g[3].z;

    CreateLight(&l[lnum++], &g[1].z, 650, 100, 100, 100);
    CreateLight(&l[lnum++], &g[3].z, 650, 300, 100, 100);
    */
    // end of circuit making

    int quit = 0;
    SDL_Event event;
    while(!quit)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    quit = 1;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    OnClick(event.button.x, event.button.y);
                    break;

                case SDL_KEYDOWN:
                    /*switch(event.key.keysym.sym)
                    {
                        case SDLK_
                    }*/
                default: {}
            }
        }
        for(int i = 0; i < gnum; i++)
        {
            Eval(&g[i]);
        }
        Render();
        SDL_Delay(16);
    }

    Free();
    return 0;
}

