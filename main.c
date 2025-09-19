#define CLAY_IMPLEMENTATION
#define CLAY_WASM
#include "./clay/clay.h"

double windowWidth = 1024, windowHeight = 768;
float modelPageOneZRotation = 0;

const uint32_t FONT_ID_BODY_16 = 0;
const uint32_t FONT_ID_TITLE_56 = 1;
const uint32_t FONT_ID_BODY_24 = 2;
const uint32_t FONT_ID_BODY_36 = 3;
const uint32_t FONT_ID_TITLE_36 = 4;
const uint32_t FONT_ID_MONOSPACE_24 = 5;

const Clay_Color COLOR_LIGHT = (Clay_Color) {244, 235, 230, 255};
const Clay_Color COLOR_LIGHT_HOVER = (Clay_Color) {224, 215, 210, 255};
const Clay_Color COLOR_RED = (Clay_Color) {168, 66, 28, 255};
const Clay_Color COLOR_RED_HOVER = (Clay_Color) {148, 46, 8, 255};
const Clay_Color COLOR_ORANGE = (Clay_Color) {225, 138, 50, 255};
const Clay_Color COLOR_BLUE = (Clay_Color) {111, 173, 162, 255};

// Colors for top stripe
const Clay_Color COLOR_TOP_BORDER_1 = (Clay_Color) {168, 66, 28, 255};
const Clay_Color COLOR_TOP_BORDER_2 = (Clay_Color) {223, 110, 44, 255};
const Clay_Color COLOR_TOP_BORDER_3 = (Clay_Color) {225, 138, 50, 255};
const Clay_Color COLOR_TOP_BORDER_4 = (Clay_Color) {236, 189, 80, 255};
const Clay_Color COLOR_TOP_BORDER_5 = (Clay_Color) {240, 213, 137, 255};

const Clay_Color COLOR_BLOB_BORDER_1 = (Clay_Color) {168, 66, 28, 255};
const Clay_Color COLOR_BLOB_BORDER_2 = (Clay_Color) {203, 100, 44, 255};
const Clay_Color COLOR_BLOB_BORDER_3 = (Clay_Color) {225, 138, 50, 255};
const Clay_Color COLOR_BLOB_BORDER_4 = (Clay_Color) {236, 159, 70, 255};
const Clay_Color COLOR_BLOB_BORDER_5 = (Clay_Color) {240, 189, 100, 255};

#define RAYLIB_VECTOR2_TO_CLAY_VECTOR2(vector) (Clay_Vector2) { .x = (vector).x, .y = (vector).y }

Clay_TextElementConfig headerTextConfig = (Clay_TextElementConfig) { .fontId = 2, .fontSize = 24, .textColor = {255, 255, 255, 255} };

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

Clay_Color ColorLerp(Clay_Color a, Clay_Color b, float amount) {
    return (Clay_Color) {
        .r = a.r + (b.r - a.r) * amount,
        .g = a.g + (b.g - a.g) * amount,
        .b = a.b + (b.b - a.b) * amount,
        .a = a.a + (b.a - a.a) * amount,
    };
}

void RendererButtonActive(Clay_String text) {
    CLAY({
        .layout = { .sizing = {CLAY_SIZING_FIXED(300) }, .padding = CLAY_PADDING_ALL(16) },
        .backgroundColor = Clay_Hovered() ? COLOR_RED_HOVER : COLOR_RED,
        .cornerRadius = CLAY_CORNER_RADIUS(10),
        .userData = FrameAllocateCustomData((CustomHTMLData) { .disablePointerEvents = true, .cursorPointer = true })
    }) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({ .fontSize = 28, .fontId = FONT_ID_BODY_36, .textColor = COLOR_LIGHT }));
    }
}

void RendererButtonInactive(Clay_String text, size_t rendererIndex) {
    CLAY({
        .layout = { .sizing = {CLAY_SIZING_FIXED(300)}, .padding = CLAY_PADDING_ALL(16) },
        .border = { .width = {2, 2, 2, 2}, .color = COLOR_RED },
        .backgroundColor = Clay_Hovered() ? COLOR_LIGHT_HOVER : COLOR_LIGHT,
        .cornerRadius = CLAY_CORNER_RADIUS(10),
        .userData = FrameAllocateCustomData((CustomHTMLData) { .disablePointerEvents = true, .cursorPointer = true })
    }) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({ .fontSize = 28, .fontId = FONT_ID_BODY_36, .textColor = COLOR_RED }));
    }
}

typedef struct
{
    Clay_Vector2 clickOrigin;
    Clay_Vector2 positionOrigin;
    bool mouseDown;
} ScrollbarData;

float animationLerpValue = -1.0f;

Clay_RenderCommandArray CreateLayout(bool mobileScreen, float lerpValue) {
    Clay_BeginLayout();
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
                    .padding = { 32, 32 } 
                } }) {
                    CLAY_TEXT(CLAY_STRING("DV"), &headerTextConfig);
                    CLAY({ .id = CLAY_ID("Spacer"), .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) } } }) {}
                    if (!mobileScreen) {
                        // CLAY({ .id = CLAY_ID("LinkExamplesOuter"), .layout = { .padding = {8, 8} } }) {
                        //     CLAY_TEXT(CLAY_STRING("GitHub"), CLAY_TEXT_CONFIG({
                        //         .userData = FrameAllocateCustomData((CustomHTMLData) {
                        //             .link = CLAY_STRING("https://github.com/DeeVi-conf")
                        //         }),
                        //         .fontId = FONT_ID_BODY_24, .fontSize = 24, .textColor = {255, 255, 255, 255} }));
                        // }
                        // CLAY({ .id = CLAY_ID("LinkDocsOuter"), .layout = { .padding = {8, 8} } }) {
                        //     CLAY_TEXT(CLAY_STRING("LinkdIn"), CLAY_TEXT_CONFIG({
                        //         .userData = FrameAllocateCustomData((CustomHTMLData) { .link = CLAY_STRING("https://github.com/nicbarker/clay/blob/main/README.md") }),
                        //         .fontId = FONT_ID_BODY_24, .fontSize = 24, .textColor = {61, 26, 5, 255} })
                        //     );
                        // }
                    }
                    // CLAY({
                    //     .layout = { .padding = {16, 16, 6, 6} },
                    //     .backgroundColor = Clay_Hovered() ? COLOR_LIGHT_HOVER : COLOR_LIGHT,
                    //     .border = { .width = {2, 2, 2, 2}, .color = COLOR_RED },
                    //     .cornerRadius = CLAY_CORNER_RADIUS(10),
                    //     .userData = FrameAllocateCustomData((CustomHTMLData) { .link = CLAY_STRING("https://discord.gg/b4FTWkxdvT") }),
                    // }) {
                    //     CLAY_TEXT(CLAY_STRING("Discord"), CLAY_TEXT_CONFIG({
                    //         .userData = FrameAllocateCustomData((CustomHTMLData) { .disablePointerEvents = true }),
                    //         .fontId = FONT_ID_BODY_24, .fontSize = 24, .textColor = {61, 26, 5, 255} }));
                    // }
                    CLAY({
                        .layout = { .padding = {16, 16, 6, 6} },
                        .backgroundColor = Clay_Hovered() ? COLOR_LIGHT_HOVER : COLOR_LIGHT,
                        .border = { .width = {2, 2, 2, 2}, .color = COLOR_RED },
                        .cornerRadius = CLAY_CORNER_RADIUS(10),
                        .userData = FrameAllocateCustomData((CustomHTMLData) { .link = CLAY_STRING("https://github.com/DeeVi-conf") }),
                    }) {
                        CLAY_TEXT(CLAY_STRING("Github"), CLAY_TEXT_CONFIG({
                            .userData = FrameAllocateCustomData((CustomHTMLData) { .disablePointerEvents = true }),
                            .fontId = FONT_ID_BODY_24, .fontSize = 24, .textColor = {61, 26, 5, 255} }));
                    }
                }
        // Clay_LayoutConfig topBorderConfig = (Clay_LayoutConfig) { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(4) }};
        // CLAY({ .id = CLAY_ID("TopBorder1"), .layout = topBorderConfig, .backgroundColor = COLOR_TOP_BORDER_5 }) {}
        // CLAY({ .id = CLAY_ID("TopBorder2"), .layout = topBorderConfig, .backgroundColor = COLOR_TOP_BORDER_4 }) {}
        // CLAY({ .id = CLAY_ID("TopBorder3"), .layout = topBorderConfig, .backgroundColor = COLOR_TOP_BORDER_3 }) {}
        // CLAY({ .id = CLAY_ID("TopBorder4"), .layout = topBorderConfig, .backgroundColor = COLOR_TOP_BORDER_2 }) {}
        // CLAY({ .id = CLAY_ID("TopBorder5"), .layout = topBorderConfig, .backgroundColor = COLOR_TOP_BORDER_1 }) {}
        CLAY({ .id = CLAY_ID("OuterScrollContainer"),
            .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) }, .layoutDirection = CLAY_TOP_TO_BOTTOM },
            .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() },
            .border = { .width = { .betweenChildren = 2 }, .color = COLOR_RED }
        }) {
            if (mobileScreen) {
                // LandingPageMobile();
                // FeatureBlocksMobile();
                // DeclarativeSyntaxPageMobile();
                // HighPerformancePageMobile(lerpValue);
                // RendererPageMobile();
            } else {
                // LandingPageDesktop();
                // FeatureBlocksDesktop();
                // DeclarativeSyntaxPageDesktop();
                // HighPerformancePageDesktop(lerpValue);
                // RendererPageDesktop();
                // DebuggerPageDesktop();
            }
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
