#define CLAY_IMPLEMENTATION
#define CLAY_WASM
#include "./clay/clay.h"

double windowWidth = 1024, windowHeight = 768;
float modelPageOneZRotation = 0;

const Clay_Color COLOR_WHITE = (Clay_Color) {255, 210, 255, 255};
const Clay_Color COLOR_BLACK = (Clay_Color) {0, 0, 0, 255};
const Clay_Color COLOR_DEBUG = (Clay_Color) {255, 0, 0, 255};

enum typeface {QUICKSAND, CALISTOGA, EVANGELION , SANS_SERIF};

#define RAYLIB_VECTOR2_TO_CLAY_VECTOR2(vector) (Clay_Vector2) { .x = (vector).x, .y = (vector).y }

Clay_TextElementConfig headerTextConfig = (Clay_TextElementConfig) { .fontId = EVANGELION, .fontSize = 24, .textColor = {255, 230, 255, 255} };

typedef struct {
    void* memory;
    uintptr_t offset;
} Arena;

Arena frameArena = {};

typedef struct d {
    Clay_String link;
    bool cursorPointer;
    bool disablePointerEvents;
} CustomHTMLData;

CustomHTMLData* FrameAllocateCustomData(CustomHTMLData data) {
    CustomHTMLData *customData = (CustomHTMLData *)(frameArena.memory + frameArena.offset);
    *customData = data;
    frameArena.offset += sizeof(CustomHTMLData);
    return customData;
}

Clay_String* FrameAllocateString(Clay_String string) {
    Clay_String *allocated = (Clay_String *)(frameArena.memory + frameArena.offset);
    *allocated = string;
    frameArena.offset += sizeof(Clay_String);
    return allocated;
}

float animationLerpValue = -1.0f;

void Title_Card() {
    CLAY({
        .id = CLAY_ID("Title_CARD"),
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT() },
            .padding = { 32, 32, 0, 0 },
            .childGap = 0 
        }
    }){
        CLAY({
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT() },
                .padding = { 32, 32, 0, 0 },
                .childGap = 0
            },
            .backgroundColor = COLOR_BLACK,
        }) {
            CLAY_TEXT(CLAY_STRING("THE\nRENDERING"), CLAY_TEXT_CONFIG({
                .fontId = EVANGELION,
                .fontSize = 120,
                .lineHeight = 80, 
                .textColor = COLOR_WHITE
            }));

            CLAY_TEXT(CLAY_STRING("PIPELINE"), CLAY_TEXT_CONFIG({
                .fontId = EVANGELION,
                .fontSize = 200,
                .lineHeight = 70,
                .textColor = COLOR_WHITE
            }));

            CLAY({ .id = CLAY_ID("Spacer"), .layout = { .sizing = { .height = CLAY_SIZING_FIXED(100) } } });

            CLAY({
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT() }, 
                    .childAlignment = { 0, CLAY_ALIGN_Y_TOP },
                }})
                {
                CLAY_TEXT(CLAY_STRING(" EPISODE:19"), CLAY_TEXT_CONFIG({
                    .fontId = SANS_SERIF,
                    .fontSize = 100,
                    .lineHeight = 100,
                    .textColor = COLOR_WHITE
                }));
                CLAY({ .id = CLAY_ID("CatchPhraseSpacer"), .layout = { .sizing = { .width = CLAY_SIZING_FIXED(250) } } });
                CLAY_TEXT(CLAY_STRING("全ての者は三角形である\nAll things are triangles"), CLAY_TEXT_CONFIG({
                    .fontId = EVANGELION,
                    .fontSize = 90,
                    .lineHeight = 80,
                    .textColor = COLOR_WHITE
                }));
            };
        };
        
        CLAY({
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = { CLAY_SIZING_FIT()},
                .padding = { 32, 32, 0, 0 },
                .childGap = 0,
                .childAlignment = {CLAY_ALIGN_X_CENTER}
            },
            .backgroundColor = COLOR_BLACK,
        }) {
            CLAY_TEXT(CLAY_STRING("武\n雅"), CLAY_TEXT_CONFIG({.fontId = EVANGELION, .fontSize = 120, .lineHeight = 90, .textColor = COLOR_WHITE}));
            CLAY_TEXT(CLAY_STRING(" "), CLAY_TEXT_CONFIG({.fontId = EVANGELION, .lineHeight = 20, .textColor = COLOR_WHITE}));
            CLAY_TEXT(CLAY_STRING("将\n勝"), CLAY_TEXT_CONFIG({.fontId = EVANGELION, .fontSize = 120, .lineHeight = 90, .textColor = COLOR_WHITE}));
        };
    }
}

void AboutMe(){
    CLAY({
        .id = CLAY_ID("About Me"),
        .layout = {
            .sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_FIT()},
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
        },
    }){
        CLAY({
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = { CLAY_SIZING_FIT()},
                .padding = { 32, 32, 0, 0 },
                .childGap = 0,
                .childAlignment = {CLAY_ALIGN_X_CENTER}
            }
        }){
            CLAY_TEXT(CLAY_STRING("天\n竺\nの\n男\n子"), CLAY_TEXT_CONFIG({.fontId = EVANGELION, .fontSize = 120, .lineHeight = 90, .textColor = COLOR_WHITE}));
        }
    }
}


Clay_RenderCommandArray CreateLayout(bool mobileScreen, float lerpValue) {
    Clay_BeginLayout();
    //Clay_SetDebugModeEnabled(true);
    CLAY({ 
        .id = CLAY_ID("OuterContainer"), 
        .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } }, 
        .backgroundColor = (Clay_Color){0, 0,0, 255} })
        {
            CLAY({ 
                .id = CLAY_ID("Header"), 
                .layout = { 
                    .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(50) }, 
                    .childAlignment = { 0, CLAY_ALIGN_Y_CENTER }, 
                    .childGap = 16, 
                    .padding = { 32, 32, 0, 0 } 
                } }) {
                    CLAY_TEXT(CLAY_STRING("DV"), &headerTextConfig);
                    CLAY({ .id = CLAY_ID("Spacer"), .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) } } }) {}
                    CLAY({
                        .layout = { .padding = {16, 16, 6, 6} },
                        .backgroundColor = Clay_Hovered() ? (Clay_Color){255, 255, 255, 255} : (Clay_Color){0, 0, 0, 255},
                        .border = { .width = {2, 2, 2, 2}, .color = (Clay_Color){255, 255, 255, 255} },
                        .cornerRadius = CLAY_CORNER_RADIUS(10),
                        .userData = FrameAllocateCustomData((CustomHTMLData) { .link = CLAY_STRING("https://github.com/DeeVi-conf") }),
                    }) {
                        CLAY_TEXT(CLAY_STRING("Github"), CLAY_TEXT_CONFIG({
                            .userData = FrameAllocateCustomData((CustomHTMLData) { .disablePointerEvents = true }),
                            .fontId = QUICKSAND, .fontSize = 24, 
                            .textColor = Clay_Hovered()? (Clay_Color){0,0,0,255} : (Clay_Color){255, 255, 255, 255}, 
                            .textAlignment =  CLAY_TEXT_ALIGN_CENTER
                        }));
                    }
                }
        CLAY({ .id = CLAY_ID("TopBorder"), .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(4) }}, .backgroundColor = COLOR_WHITE }) {}
        CLAY({ .id = CLAY_ID("OuterScrollContainer"),
            .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) }, .layoutDirection = CLAY_TOP_TO_BOTTOM, .childGap = 0 },
            .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset(), },
            //.border = { .width = { .betweenChildren = 2 }, .color = COLOR_WHITE }
            }) {
                CLAY({ .id = CLAY_ID("Spacer"), .layout = { .sizing = { .height = CLAY_SIZING_FIXED(200) } }, .backgroundColor = COLOR_DEBUG });
                Title_Card();
                CLAY({ .id = CLAY_ID("Spacer"), .layout = { .sizing = { .height = CLAY_SIZING_FIXED(250) } }, .backgroundColor = COLOR_DEBUG });
                AboutMe();
                CLAY({ .id = CLAY_ID("Spacer"), .layout = { .sizing = { .height = CLAY_SIZING_FIXED(200) } }, .backgroundColor = COLOR_DEBUG });
            }
        }

    return Clay_EndLayout();
}

bool debugModeEnabled = false;

CLAY_WASM_EXPORT("SetScratchMemory") void SetScratchMemory(void * memory) {
    frameArena.memory = memory;
}

CLAY_WASM_EXPORT("UpdateDrawFrame") Clay_RenderCommandArray UpdateDrawFrame(float width, float height, float mouseWheelX, float mouseWheelY, float mousePositionX, float mousePositionY, bool isTouchDown, bool isMouseDown, bool arrowKeyDownPressedThisFrame, bool arrowKeyUpPressedThisFrame, bool dKeyPressedThisFrame, float deltaTime) {
    frameArena.offset = 0;
    windowWidth = width;
    windowHeight = height;
    Clay_SetLayoutDimensions((Clay_Dimensions) { width, height });
    Clay_ScrollContainerData scrollContainerData = Clay_GetScrollContainerData(Clay_GetElementId(CLAY_STRING("OuterScrollContainer")));
    Clay_LayoutElementHashMapItem *perfPage = Clay__GetHashMapItem(Clay_GetElementId(CLAY_STRING("PerformanceOuter")).id);
    // NaN propagation can cause pain here
    float perfPageYOffset = perfPage->boundingBox.y + scrollContainerData.scrollPosition->y;
    if (deltaTime == deltaTime && (perfPageYOffset < height && perfPageYOffset + perfPage->boundingBox.height > 0)) {
        animationLerpValue += deltaTime;
        if (animationLerpValue > 1) {
            animationLerpValue -= 2;
        }
    }

    if (dKeyPressedThisFrame) {
        debugModeEnabled = !debugModeEnabled;
        Clay_SetDebugModeEnabled(debugModeEnabled);
    }
    Clay_SetCullingEnabled(false);
    Clay_SetExternalScrollHandlingEnabled(true);

    Clay__debugViewHighlightColor = (Clay_Color) {105,210,231, 120};

    Clay_SetPointerState((Clay_Vector2) {mousePositionX, mousePositionY}, isMouseDown || isTouchDown);

    if (arrowKeyDownPressedThisFrame) {
        if (scrollContainerData.contentDimensions.height > 0) {
            scrollContainerData.scrollPosition->y = scrollContainerData.scrollPosition->y - 50;
        }
    } else if (arrowKeyUpPressedThisFrame) {
        if (scrollContainerData.contentDimensions.height > 0) {
            scrollContainerData.scrollPosition->y = scrollContainerData.scrollPosition->y + 50;
        }
    }

    Clay_UpdateScrollContainers(isTouchDown, (Clay_Vector2) {mouseWheelX, mouseWheelY}, deltaTime);
    bool isMobileScreen = windowWidth < 750;
    if (debugModeEnabled) {
        isMobileScreen = windowWidth < 950;
    }
    return CreateLayout(isMobileScreen, animationLerpValue < 0 ? (animationLerpValue + 1) : (1 - animationLerpValue));
    //----------------------------------------------------------------------------------
}

// Dummy main() to please cmake - TODO get wasm working with cmake on this example
int main() {
    return 0;
}
