#include <GL/glew.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include "GUI.h"
#include "Util.h"
#include "Planet.h"
#include "SurfaceGenerator.h"
#include "PostProcessor.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static GLuint textInputTex = 0, alphaTex = 0, symbolTex = 0;
static ImFont* fontDefault = nullptr;
static char inputTextBuffer[128] = "";

void initImGUI(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig fontConfig;
    fontConfig.PixelSnapH = true;
    fontConfig.OversampleH = 1;
    fontConfig.OversampleV = 1;

    fontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/BoldPixels.ttf", 16.0f, &fontConfig);
    if (fontDefault != nullptr) {
        io.FontDefault = fontDefault;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;
}

GLuint loadPixelTexture(const char* filepath) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 4);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    return textureID;
}

// Col 0 = normal / raised state of button
// Col 1 = pressed / flat state of button
bool drawRowButton(const char* str_id, GLuint textureID, int rowIndex, int colBlock = 0, ImVec2 displaySize = ImVec2(32, 32), std::string tooltip = "",
                   int totalCols = 8, int totalRows = 26, bool allowMouseClick = true) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGuiID id = window->GetID(str_id);

    ImRect totalBb(pos, ImVec2(pos.x + displaySize.x, pos.y + displaySize.y));
    ImGui::ItemSize(totalBb, 0.0f);
    if (!ImGui::ItemAdd(totalBb, id)) return false;

    bool hovered = false, held = false, clicked = false;
    if (allowMouseClick) clicked = ImGui::ButtonBehavior(totalBb, id, &hovered, &held);

    int subCol = 0;
    if (held) subCol = 3;
    else if (hovered) {
        subCol = 1;
        if (!tooltip.empty()) ImGui::SetTooltip(tooltip.c_str(), "formatted");
    }

    int actualCol = allowMouseClick ? (colBlock * 4 + subCol) : colBlock;

    float uStep = 1.0f / static_cast<float>(totalCols);
    float vStep = 1.0f / static_cast<float>(totalRows);

    ImVec2 uv0 = ImVec2(actualCol * uStep, rowIndex * vStep);
    ImVec2 uv1 = ImVec2((actualCol + 1) * uStep, (rowIndex + 1) * vStep);

    drawList->AddImage((ImTextureID)(uintptr_t)textureID, pos,ImVec2(pos.x + displaySize.x, pos.y + displaySize.y), uv0, uv1);

    return clicked;
}

bool drawSingleSpriteInputText(const char* str_id, const char* hint, char* buf, size_t buf_size, GLuint textureID,
    ImVec2 displaySize = ImVec2(160, 32), ImGuiInputTextFlags flags = 0) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGuiID id = window->GetID(str_id);

    ImRect totalBb(pos, ImVec2(pos.x + displaySize.x, pos.y + displaySize.y));
    ImGui::ItemSize(totalBb, style.FramePadding.y);
    if (!ImGui::ItemAdd(totalBb, id)) return false;

    drawList->AddImage((ImTextureID)(uintptr_t)textureID, pos, ImVec2(pos.x + displaySize.x, pos.y + displaySize.y), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));

    float paddingX = 18.0f;
    float paddingY = (displaySize.y - ImGui::GetFontSize()) * 0.48f;

    ImGui::SetCursorScreenPos(ImVec2(pos.x + paddingX, pos.y + paddingY));
    ImGui::SetNextItemWidth(displaySize.x - (paddingX * 2.0f));

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(0.55f, 0.55f, 0.60f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    bool valueSubmitted = ImGui::InputTextWithHint(str_id, hint, buf, buf_size, flags);

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);

    return valueSubmitted;
}

auto drawWASDKey = [](const char* id, GLuint tex, int row, ImVec2 size, int glfwKey) {
    bool isTyping = ImGui::GetIO().WantCaptureKeyboard;
    bool isKeyPressed = !isTyping && (glfwGetKey(window, glfwKey) == GLFW_PRESS);
    drawRowButton(id, tex, row, isKeyPressed ? 3 : 0, size, "", 8, 26, false);
};

void drawHUD() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (alphaTex == 0)  alphaTex = loadPixelTexture("assets/textures/kb_dark_alphanumeric.png");
    if (symbolTex == 0) symbolTex = loadPixelTexture("assets/textures/kb_dark_symbols.png");
    if (textInputTex == 0) textInputTex = loadPixelTexture("assets/textures/kb_dark_text.png");

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const float pad = 16.0f;

    // top right buttons + input field
    const ImVec2 iconSize(64.0f, 64.0f);
    const ImVec2 pillSize(224.0f, 64.0f);

    const float topGap = -12.0f;
    const float clusterWidth = pillSize.x + topGap + (iconSize.x * 2.0f) + (topGap * 2.0f);

    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - clusterWidth - pad * 2, viewport->WorkPos.y + pad));
    ImGui::SetNextWindowBgAlpha(0.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("##TopRightHUD", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize);

    ImVec2 topStartPos = ImGui::GetCursorScreenPos();

    if (drawSingleSpriteInputText("##seedInput", "Planet Seed", inputTextBuffer, sizeof(inputTextBuffer), textInputTex, pillSize, ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (strlen(inputTextBuffer) > 0) {
            params.seed = hashSeed(inputTextBuffer);
            randomiser.setSeed(params.seed);
            planet.build();
        }
    }

	// randomise button
    float randBtnX = topStartPos.x + pillSize.x;
    ImGui::SetCursorScreenPos(ImVec2(randBtnX, topStartPos.y));
    if (drawRowButton("##keyRand", symbolTex, 3, 1, iconSize, "Randomise seed")) {
        params.seed = static_cast<int>(glfwGetTime() * 100000.0f);
        randomiser.setSeed(params.seed);
        planet.build();
    }

	// export picture button
    float infoBtnX = randBtnX + iconSize.x + topGap;
    ImGui::SetCursorScreenPos(ImVec2(infoBtnX, topStartPos.y));
    if (drawRowButton("##keyPhoto", symbolTex, 16, 1, iconSize, "Export planet image")) {
        postProcessor.exportPixelatedImage("planet_" + std::to_string(params.seed) + ".png", pixelationShader);
    }

    ImGui::End();
    ImGui::PopStyleVar(3);

    // bottom left rotation buttons 
    const ImVec2 keySize(64.0f, 64.0f);
    const float keyGap = -18.0f;
    const float rowGap = -12.0f;
    const float wasdWidth = keySize.x * 3.0f + keyGap * 2.0f;

    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + pad, viewport->WorkPos.y + viewport->WorkSize.y - pad - (keySize.y * 2.0f + rowGap + 30.0f)));
    ImGui::SetNextWindowBgAlpha(0.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y));

    ImGui::Begin("##BottomLeftHUD", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize);

    ImVec2 startPos = ImGui::GetCursorScreenPos();

    // w key
    float wPosX = startPos.x + (keySize.x + keyGap);
    ImGui::SetCursorScreenPos(ImVec2(wPosX, startPos.y));
    drawWASDKey("##keyW", alphaTex, 22, keySize, GLFW_KEY_W);

    // a key
    float row2Y = startPos.y + keySize.y + rowGap;
    ImGui::SetCursorScreenPos(ImVec2(startPos.x, row2Y));
    drawWASDKey("##keyA", alphaTex, 0, keySize, GLFW_KEY_A);

    // s key
    ImGui::SetCursorScreenPos(ImVec2(startPos.x + keySize.x + keyGap, row2Y));
    drawWASDKey("##keyS", alphaTex, 18, keySize, GLFW_KEY_S);

    // d key
    ImGui::SetCursorScreenPos(ImVec2(startPos.x + (keySize.x + keyGap) * 2.0f, row2Y));
    drawWASDKey("##keyD", alphaTex, 3, keySize, GLFW_KEY_D);

    // caption
    ImGui::SetCursorScreenPos(ImVec2(startPos.x + 12.0f, row2Y + keySize.y + 6.0f));
    ImGui::Text("Control rotation");

    ImGui::End();
    ImGui::PopStyleVar(3);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}