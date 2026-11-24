#include "raylib.h"
#include "raymath.h"
#include "textbox.h"
#include "turtle.h"
#include "linegenerator.h"
#include "lineparser.h"
#include "fileselector.h"
#include <fstream>
#include "json.hpp"
using nlohmann::json;

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_FONT_ICONS_BAKING
#include "raygui.h"

constexpr int screenWidth = 1600;
constexpr int screenHeight = 900;

const int ruleLength = 32;
const int rulesCount = 8;

const std::string basicLSystem = "{"
	"\"axiom\": \"x\","
	"\"rules\": {"
	"	\"f\": \"f\","
	"	\"x\": \"f[-x][+x]\""
	"},"
	"\"iterations\": 7.0,"
	"\"angle\": 30.0,"
	"\"decay\": 0.78"
"}";

Vector2 mousePos{0.0, 0.0};
Vector2 screenCenter{screenWidth / 2.0, screenHeight / 2.0};
float scrollSpeed = 0.1;
float zoom = 1.0;
bool isFullscreen = false;
bool updated = true;

Turtle turtle{200.0, 600.0, 90.0};
LineGenerator lineGen{};
LineParser parser{&turtle};

void loadLSystem(json lsystem);
void loadLSystem(std::string content);
void saveLSystem(json lsystem, std::string saveTo);
void resetLSystem();

std::string loadedSystem = "";
char loadedName[256];

void clearLSystem();
void setupLSystem(std::string sysAxiom, std::map<char, std::string> sysRules);
void createLSystem(int sysIterations, float sysAngle, float sysDecay);
void updateLSystem();
void returnCamera();
void generate();

char axiom[ruleLength] = "x";
char ruleNames[rulesCount][2] = {"f", "x"};
char rules[rulesCount][ruleLength] = {"f", "f[-x][+x]"};
float iterations = 8.0f;
float angle = 30.0f;
float decay = 0.78f;

int main() {
    InitWindow(screenWidth, screenHeight, "L-System");
    SetTargetFPS(60);
    GuiLoadStyle("style_terminal.rgs");
    std::string lsystemsDir{std::string{GetApplicationDirectory()}+"systems"};
    FileSelector selector{lsystemsDir};
    selector.setSize(Vector2{640.0, 480.0});
    selector.setPosition(screenCenter - selector.getSize() / 2.0f);
    bool saveDialogEnabled = false;
    bool isViewportFocused = IsWindowFocused();
    // UI properties
    bool axiomTBEditMode = false;
    bool ruleNamesTBEditMode[rulesCount];
    for(int i = 0; i < rulesCount; i++) ruleNamesTBEditMode[i] = false;
    bool rulesTBEditMode[rulesCount];
    for(int i = 0; i < rulesCount; i++) rulesTBEditMode[i] = false;
    bool saveDialogTextBox = false;
    // Text box preload
    TextBox iterationsBox{
        Vector2{0.0f, 0.0f}, Vector2{0.0f, 0.0f}, "Iter.", 16
    };
    TextBox angleBox{
        Vector2{0.0f, 0.0f}, Vector2{0.0f, 0.0f}, "Angle", 16
    };
    TextBox decayBox{
        Vector2{0.0f, 0.0f}, Vector2{0.0f, 0.0f}, "Decay", 16
    };
    // first load
    resetLSystem();
    generate();
    // Game-loop
    while (!WindowShouldClose()) {
        Vector2 boxSize = {272.0, screenCenter.y * 2};
        /* --- Update --- */ 
        // Update L-System
        if(selector.getSelected() > -1) {
            char* filePath = selector.getFile(selector.getSelected());
            loadedSystem = std::string{filePath};
            std::string fileName{loadedSystem};
            fileName.erase(0, lsystemsDir.size() + 1);
            for(int i = 0; i < 256; i++) {
                if(loadedName[i] == '\0') break;
                loadedName[i] = '\0';
            }
            for(unsigned int i = 0; i < fileName.size(); i++) loadedName[i] = fileName[i];
            std::ifstream file{filePath};
            json content = json::parse(file);
            loadLSystem(content);
            selector.setSelected(-1);
        }
        if(isViewportFocused == false && IsWindowFocused() == true) selector.Update();
        isViewportFocused = IsWindowFocused();
        if(updated) {
            updated = false;
            iterationsBox.setString(std::to_string(iterations));
            angleBox.setString(std::to_string(angle));
            decayBox.setString(std::to_string(decay));
        }
        // Mouse position
        auto curMousePos = GetMousePosition();
        bool isMouseOnUI = (
            selector.isEnable() || saveDialogEnabled || (CheckCollisionPointRec(
                curMousePos, (Rectangle){0.0, 0.0, boxSize.x, boxSize.y}
            ) || CheckCollisionPointRec(
                curMousePos, (Rectangle){screenCenter.x * 2 - boxSize.x, 0.0, boxSize.x, boxSize.y}
            ))
        );
        if(!isMouseOnUI && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            turtle.transform.orgin.x += curMousePos.x - mousePos.x;
            turtle.transform.orgin.y += curMousePos.y - mousePos.y;
        }
        mousePos = curMousePos;
        // Zooming
        float curScaling = GetMouseWheelMove() * scrollSpeed;
        if(!isMouseOnUI && curScaling != 0.0) {
            float prev_zoom = zoom;
            zoom = exp(log(zoom) + curScaling);
            turtle.setScale(zoom);
            Vector2 newPos = zoom * (turtle.transform.orgin - mousePos) / prev_zoom;
            turtle.transform.orgin = newPos + mousePos;
        }
        // Fullscreen
        if(IsKeyPressed(KEY_F4)) {
            ToggleFullscreen();
            if(isFullscreen) {
                isFullscreen = false;
                screenCenter = Vector2{screenWidth / 2.0, screenHeight / 2.0};
            } else {
                isFullscreen = true;
                screenCenter = Vector2{
                    (float)(GetScreenWidth() / 2), (float)(GetScreenHeight() / 2)
                };
            }
            selector.setPosition(screenCenter - selector.getSize() / 2.0f);
        }
        /* --- Render --- */ 
        BeginDrawing();
        ClearBackground(BLACK);
        turtle.draw();
        /* - UI - */
        Color boxColor{0, 42, 12, 255};
        DrawRectangle(0.0, 0.0, boxSize.x, boxSize.y, boxColor);
        DrawLineEx(
            Vector2{boxSize.x, 0.0},
            Vector2{boxSize.x, boxSize.y},
            2.0, GREEN
        );
        DrawRectangle(screenCenter.x * 2 - boxSize.x, 0.0, boxSize.x, boxSize.y, boxColor);
        DrawLineEx(
            Vector2{screenCenter.x * 2 - boxSize.x, 0.0},
            Vector2{screenCenter.x * 2 - boxSize.x, boxSize.y},
            2.0, GREEN
        );
        // Left box (L-System config)
        DrawText("L-System 1.666", 16, 16, 20, GREEN);
        float margin = 16.0f;
        float edgeMargin = 2.0f * screenCenter.x - boxSize.x + margin;
        float gap = 4.0f;
        float boxWidth = 30.0f;
        float startY = 64.0f;
        int num = 1;
        auto offsetNum = [](float start, float width, int* num){
            return (float)(start + ((*num)++) * width);
        };
        if(GuiTextBox(
            (Rectangle){
                margin, startY, boxSize.x - (float)(margin * 2.0), boxWidth
            }, axiom, 32, axiomTBEditMode
        )) axiomTBEditMode = !axiomTBEditMode;
        int occupied = 1;
        for(int i = 0; i < rulesCount; i++) if(ruleNames[i][0] != '\0') occupied = i+2;
        if(occupied > rulesCount) occupied = rulesCount;
        float boxPosY = offsetNum(startY, boxWidth + gap, &num);
        for(int i = 0; i < occupied; i++) {
            if(GuiTextBox(
                (Rectangle){
                    margin, boxPosY, boxWidth, boxWidth
                }, ruleNames[i], 2, ruleNamesTBEditMode[i]
            )) ruleNamesTBEditMode[i] = !(ruleNamesTBEditMode[i]);
            if(GuiTextBox(
                (Rectangle){
                    margin + boxWidth + gap, boxPosY,
                    boxSize.x - (float)(2.0 * margin + boxWidth + gap), boxWidth
                }, rules[i], 32, rulesTBEditMode[i]
            )) rulesTBEditMode[i] = !(rulesTBEditMode[i]);
            boxPosY = offsetNum(startY, boxWidth + gap, &num);
        }
        iterationsBox.setPosition({margin, offsetNum(startY, boxWidth + gap, &num)});
        iterationsBox.setSize({boxSize.x - (float)(72.0), boxWidth});
        if(iterations != TextToFloat(iterationsBox.getString().c_str())) {
            iterations = TextToFloat(iterationsBox.getString().c_str());
            updateLSystem();
        };
        iterationsBox.draw();
        angleBox.setPosition({margin, offsetNum(startY, boxWidth + gap, &num)});
        angleBox.setSize({boxSize.x - (float)(72.0), boxWidth});
        if(angle != TextToFloat(angleBox.getString().c_str())) {
            angle = TextToFloat(angleBox.getString().c_str());
            updateLSystem();
        };
        angleBox.draw();
        decayBox.setPosition({margin, offsetNum(startY, boxWidth + gap, &num)});
        decayBox.setSize({boxSize.x - (float)(72.0), boxWidth});
        if(decay != TextToFloat(decayBox.getString().c_str())) {
            decay = TextToFloat(decayBox.getString().c_str());
            updateLSystem();
        };
        decayBox.draw();
        if(
            GuiButton(
                (Rectangle){
                    margin, offsetNum(startY, boxWidth + gap, &num),
                    boxSize.x - (float)(margin * 2.0), boxWidth
                },
                "Generate L-System"
            )
        ) generate();
        if(
            GuiButton(
                (Rectangle){
                    margin, offsetNum(startY, boxWidth + gap, &num),
                    boxSize.x - (float)(margin * 2.0), boxWidth
                },
                "Return camera"
            )
        ) returnCamera();
        // Right box (File loader)
        DrawText("File loader", edgeMargin, 16, 20, GREEN);
        num = 0;
        if(GuiButton(
            (Rectangle){
                edgeMargin,
                offsetNum(startY, boxWidth + gap, &num),
                boxSize.x - (float)(margin * 2.0), boxWidth
            }, "New L-System"
        )) resetLSystem();
        if(GuiButton(
            (Rectangle){
                edgeMargin,
                offsetNum(startY, boxWidth + gap, &num),
                boxSize.x - (float)(margin * 2.0), boxWidth
            }, "Load L-System"
        ) && !saveDialogEnabled) selector.toggleEnable();
        if(GuiButton(
            (Rectangle){
                edgeMargin,
                offsetNum(startY, boxWidth + gap, &num),
                boxSize.x - (float)(margin * 2.0), boxWidth
            }, "Save L-System"
        ) && !selector.isEnable()) saveDialogEnabled = true;
        std::string fileName = "UNSAVED";
        if(loadedSystem != "") {
            fileName = loadedSystem;
            fileName.erase(0, lsystemsDir.size()+1);
        }
        DrawText(
            fileName.c_str(), screenCenter.x * 2.0f - boxSize.x + margin,
            offsetNum(startY, boxWidth + gap, &num), 20, GREEN
        );
        // Marker
        DrawCircle(screenCenter.x, screenCenter.y, 2.0, WHITE);
        if(selector.isEnable() || saveDialogEnabled) {
            DrawRectangle(
                0, 0, screenCenter.x * 2.0f, screenCenter.y * 2.0f, {0, 12, 0, 160}
            );
            if(saveDialogEnabled) {
                Vector2 dialogSize{400.0, 40.0};
                Vector2 dialogPosition{screenCenter - dialogSize / 2.0f};
                float margin = 4.0f;
                if(GuiButton(
                    (Rectangle){
                        dialogSize.x + dialogPosition.x - 20.0f,
                        dialogPosition.y - (dialogSize.y / 2.0f + 2.0f * margin) - 20.0f,
                        20.0, 20.0
                    }, GuiIconText(ICON_CROSS_SMALL, NULL)
                )) saveDialogEnabled = false;
                if(GuiTextBox(
                    (Rectangle){
                        dialogPosition.x, dialogPosition.y - (dialogSize.y / 2.0f + margin),
                        dialogSize.x, dialogSize.y
                    }, loadedName, 255, saveDialogTextBox
                )) saveDialogTextBox = !saveDialogTextBox;
                if(GuiButton(
                    (Rectangle){
                        dialogPosition.x, dialogPosition.y + (dialogSize.y / 2.0f + margin),
                        dialogSize.x, dialogSize.y
                    }, "Save L-System"
                )) {
                    saveDialogEnabled = false;
                    if(loadedName[0] != '\0') {
                        // Convertation
                        std::string axiomStr{axiom};
                        std::map<std::string, std::string> rulesMap{};
                        for(int i = 0; i < occupied - 1; i++) {
                            rulesMap.insert({std::string{ruleNames[i][0]}, rules[i]});
                        }
                        // Json
                        json content = {};
                        content.push_back(json::object_t::value_type("axiom", axiomStr));
                        content.push_back(json::object_t::value_type("rules", rulesMap));
                        content.push_back(
                            json::object_t::value_type("iterations", roundf(iterations))
                        );
                        content.push_back(
                            json::object_t::value_type("angle", roundf(angle))
                        );
                        content.push_back(
                            json::object_t::value_type("decay", roundf(decay * 100) / 100)
                        );
                        saveLSystem(content, lsystemsDir + "/" + std::string{loadedName});
                        loadedSystem = lsystemsDir + "/" + std::string{loadedName};
                        selector.Update();
                    }
                }
            }
        }
        selector.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void loadLSystem(json lsystem) {
    clearLSystem();
    // axiom
    std::string axiomVal = json::string_t{lsystem["axiom"]}.c_str();
    for(unsigned int i = 0; i < axiomVal.size(); i++) {
        if(i == ruleLength || axiomVal[i] == '\0') break;
        axiom[i] = axiomVal[i];
    }
    // rules
    auto rulesObj = lsystem["rules"].get<json::object_t>();
    int ruleNum = 0;
    for(auto it = rulesObj.begin(); it != rulesObj.end(); it++) {
        if(ruleNum + 1 == rulesCount) break;
        // name
        ruleNames[ruleNum][0] = (it->first)[0];
        // rule
        std::string rule = json::string_t{lsystem["rules"][it->first]};
        for(unsigned int i = 0; i < rule.size(); i++) {
            rules[ruleNum][i] = rule[i];
        }
        ruleNum += 1;
    }
    // parameters
    iterations = json::number_float_t{lsystem["iterations"]};
    angle = json::number_float_t{lsystem["angle"]};
    decay = json::number_float_t{lsystem["decay"]};

    updated = true;
    generate();
}

void loadLSystem(std::string content) {
    auto J = json::parse(content);
    loadLSystem(J);
}

void saveLSystem(json lsystem, std::string saveTo) {
    std::ofstream jsonFile{saveTo};
    jsonFile << lsystem;
    jsonFile.close();
}

void resetLSystem() {
    loadLSystem(basicLSystem);
    loadedSystem = "";
    for(int i = 0; i < 256; i++) {
        if(loadedName[i] == '\0') break;
        loadedName[i] = '\0';
    }
}

void clearLSystem() {
    for(int i = 0; i < ruleLength; i++) axiom[i] = '\0';
    for(int i = 0; i < rulesCount; i++) {
        ruleNames[i][0] = '\0';
        for(int j = 0; j < ruleLength; j++) {
            rules[i][j] = '\0';
        }
    }
}

void setupLSystem(std::string sysAxiom, std::map<char, std::string> sysRules) {
    lineGen.setAxiom(sysAxiom);
    lineGen.setRules(sysRules);
}

void createLSystem(int sysIterations, float sysAngle, float sysDecay) {
    turtle.clear();
    std::string line = lineGen.getLine(sysIterations);
    parser.setMaxLightLevel(lineGen.getLightLevel(line)+1);
    parser.setAngle(sysAngle);
    parser.setDecay(sysDecay);
    parser.parse(line);
}

void updateLSystem() {
    createLSystem(roundf(iterations), roundf(angle), roundf(decay * 100) / 100);
}

void returnCamera() {
    turtle.setScale(1.0);
    zoom = screenCenter.y / turtle.getSystemSize().y;
    turtle.setScale(zoom);
    turtle.transform.orgin = screenCenter - turtle.getSystemCenter();
}

void generate() {
    // Convertation
    std::string axiomStr{axiom};
    std::map<char, std::string> rulesMap;
    for(int i = 0; i < rulesCount; i++) rulesMap.insert({ruleNames[i][0], rules[i]});
    // Generation
    setupLSystem(axiomStr, rulesMap);
    updateLSystem();
    returnCamera();
}
