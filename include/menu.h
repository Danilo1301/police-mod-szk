#pragma once

#include "aml-psdk/game_sa/plugin.h"
#include "aml-psdk/game_sa/engine/Sprite2d.h"
#include "aml-psdk/game_sa/entity/Placeable.h"
#include "aml-psdk/gta_base/RGBA.h"
#include "aml-psdk/gta_base/Vector.h"
#include "aml-psdk/renderware/RwTexture.h"
#include "mod/logger.h"

#include <functional>
#include <string>
#include <vector>

struct GameEntity
{
    void* ptr;
    int ref;
};

enum ScreenLogType
{
    Info,
    Warning,
    Error,
    Special
};

template <typename... Args> struct IEventListener
{
    using Callback = std::function<void(Args...)>;
    using ConditionCallback = std::function<bool(Args...)>;

    virtual ~IEventListener() = default;

    virtual void Add(const Callback& cb) = 0;
    virtual void AddRef(void* ref, const Callback& cb) = 0;
    virtual void AddOnce(const Callback& cb) = 0;
    virtual void AddUntil(const ConditionCallback& cb) = 0;
    virtual void Emit(Args... args) = 0;
    virtual void Remove(void* ref) = 0;
    virtual void Clear() = 0;

    virtual int GetListenersCount() = 0;
};

enum GameFontAlignment : unsigned char
{
    ALIGN_CENTER,
    ALIGN_LEFT,
    ALIGN_RIGHT
};

enum GameFontStyle : unsigned char
{
    FONT_GOTHIC,
    FONT_SUBTITLES,
    FONT_MENU,
    FONT_PRICEDOWN
};

struct IFontStyle
{
    CRGBA color = CRGBA(255, 255, 255);
    float size = 1.0f;
    CVector2D scale = CVector2D(1, 1);
    float opacity = 1.0f;
    GameFontAlignment align = GameFontAlignment::ALIGN_CENTER;
    GameFontStyle style = GameFontStyle::FONT_SUBTITLES;
    int dropShadowPosition = 1;
    CRGBA dropColor = CRGBA(0, 0, 0);
};

struct ITexture
{
    RwTexture* texture = nullptr;
};

struct MenuMargin
{
    float top = 0;
    float left = 0;
    float bottom = 0;
    float right = 0;
};

enum class HorizontalAlign
{
    Left = 0,
    Middle,
    Right
};

enum class VerticalAlign
{
    Top = 0,
    Middle,
    Bottom
};

enum IContainerState
{
    Normal,
    Clicked,
    Disabled
};

struct IStyle
{
    std::string position = "absolute";

    std::string left = "0px";
    std::string right = "auto";
    std::string top = "0px";
    std::string bottom = "auto";

    std::string width = "200px";
    std::string height = "200px";

    CVector2D scale = CVector2D(1, 1);
    float opacity = 1.0f;

    MenuMargin margin = MenuMargin();

    CRGBA backgroundColor = CRGBA(255, 255, 255, 0);
    CRGBA backgroundColorClicked = CRGBA(255, 255, 255, 0);
    CRGBA imageColor = CRGBA(255, 255, 255, 255);

    std::string backgroundImage = "";

    CVector2D transformOrigin = CVector2D(-1.0f, -1.0f);

    HorizontalAlign textHorizontalAlign = HorizontalAlign::Left;
    VerticalAlign textVerticalAlign = VerticalAlign::Middle;

    CVector2D textOffset = CVector2D(0, 0);
};

enum IBlockType
{
    None,
    BlockThisAndChildren,
    BlockInheritedFromParent
};

struct IContainer
{
    std::string tag;
    std::string text;

    bool visible = true;
    IBlockType block = IBlockType::None;

    IFontStyle fontStyle;
    IStyle style;

    bool canBlockTouchEvents = false;
    bool canClickThrough = false;
    bool canDrag = false;
    bool drawBoundings = false;
    bool hideWhenPaused = false;

    IEventListener<>* onClick;
    IEventListener<>* onHold;
    IEventListener<IContainerState>* onStateChanged;

    IEventListener<>* onPreUpdateTransform;
    IEventListener<>* onPostUpdateTransform;

    IContainerState state = IContainerState::Normal;

    virtual IContainer* AddChild_I(const std::string& tag) = 0;
    virtual IContainer* FindChild_I(const std::string& tag) = 0;
    virtual void SetDisabled(bool disabled) = 0;

    // bigger number = on top
    virtual void SetPriority(int priority) = 0;

    virtual void Destroy() = 0;

    void SetRelativePosition(float x, float y)
    {
        style.left = std::to_string((int)std::round(x)) + "px";
        style.right = "auto";
        style.top = std::to_string((int)std::round(y)) + "px";
        style.bottom = "auto";
    }
};

struct IMenuItem
{
    IEventListener<>* onValueChange;

    virtual bool GetBoolValue() = 0;

    virtual void AddOption(int value, const std::string& displayText) = 0;
    virtual int GetCurrentOptionValue() = 0;
    virtual void SetOptionIndex(int index) = 0;

    virtual IContainer* GetContainer() = 0;

    virtual void AddColorPreview(CRGBA* color) = 0;

    virtual void AddIcon(const std::string& pngFilePath) = 0;
    virtual void SetHoldToChange(bool hold) = 0;
};

struct IWindow
{
    std::string title = "Titulo";
    std::string subTitle = "";
    float width = 800;
    CRGBA windowColor = CRGBA(0, 150, 255);
    bool blocked = false;
    int maxItemsPerPage = 5;

    virtual IMenuItem* AddCheckbox(const std::string& text, bool* pValue) = 0;
    virtual IMenuItem* AddFloatOptions(const std::string& text, float* pValue, float min, float max, float step) = 0;
    virtual IMenuItem* AddIntOptions(const std::string& text, int* pValue, int min, int max, int step) = 0;
    virtual IMenuItem* AddOptions(const std::string& text, float optionsWidth = 450.0f) = 0;
    virtual IMenuItem* AddItem(const std::string& text) = 0;
    virtual IMenuItem* AddButton(const std::string& text, std::function<void()> onClick) = 0;
    virtual IMenuItem* AddSlider(const std::string& text, float* pValue, float minValue, float maxValue, int decimals) = 0;
    virtual IMenuItem* AddCustomItem(const std::string& text, float height) = 0;

    virtual void SetCloseButtonVisible(bool visible) = 0;
    virtual void Close() = 0;

    virtual IWindow* OpenColorMenu(CRGBA* color) = 0;

    IEventListener<>* onClose;
};

struct IWidget
{
    IEventListener<>* onClick = 0;

    virtual void SetVisible(bool visible) = 0;
    virtual void SetPosition(float x, float y) = 0;
    virtual void SetSize(float size) = 0;
    virtual float GetSize() = 0;
    virtual void Destroy() = 0;
    virtual IContainer* GetContainer() = 0;
};

struct IAudio
{
    virtual ~IAudio() = default;

    virtual void Play() = 0;
    virtual void Stop() = 0;
    virtual void SetLoop(bool loop) = 0;
    virtual bool Finished() = 0;
    virtual bool Loaded() = 0;
    virtual bool Is3D() = 0;
    virtual void AttachToCPlaceable(CPlaceable* ptr) = 0;
    virtual void SetVolume(float volume) = 0;
};

struct IRadarBlip
{
    CVector worldPosition = CVector(0, 0, 0);
    ITexture* texture = nullptr;
    int spriteId = 0;
    CRGBA color = CRGBA(255, 255, 255);
    bool dontDrawOnBorder = true;
    float size = 50.0f;

    virtual void Destroy();
};

struct IMenuSZK
{
    virtual IWindow* CreateWindow(float x, float y, float width, const std::string& title, const std::string& subtitle) = 0;

    virtual void AddCellphoneScript(const std::string& text, const std::string& iconPath, std::function<void()> fn) = 0;

    IEventListener<GameEntity>* onPedAdded = 0;
    IEventListener<GameEntity>* onPedRemoved = 0;

    IEventListener<GameEntity>* onVehicleAdded = 0;
    IEventListener<GameEntity>* onVehicleRemoved = 0;

    IEventListener<>* onPlayerReady = 0;

    IEventListener<unsigned int>* onMenuProcess = 0;

    // i think its safe to can call opcodes here
    IEventListener<unsigned int>* onScriptProcess = 0;

    IEventListener<unsigned int>* onDrawBeforeMenu = 0;
    IEventListener<unsigned int>* onDrawAfterMenu = 0;

    IEventListener<>* onPostDrawRadar = 0;

    virtual std::vector<GameEntity> GetPeds() = 0;
    virtual std::vector<GameEntity> GetVehicles() = 0;

    virtual IWidget* CreateWidget(float x, float y, float size, const std::string& bgImage, const std::string& image) = 0;

    virtual RwTexture* LoadTexture(const std::string& pngFilePath, bool cache = true) = 0;
    virtual ITexture* GetOrLoadTexture(const std::string& pngFilePath) = 0;

    virtual IContainer* GetMainContainer() = 0;

    virtual void DrawText(const std::string& text, CVector2D& position, IFontStyle& style) = 0;
    virtual void DrawRect(CVector2D position, CVector2D size, CRGBA color) = 0;
    virtual void DrawTexture(ITexture* texture, CVector2D position, CVector2D size, CRGBA color) = 0;

    virtual IRadarBlip* CreateBlip(ITexture* texture, CVector worldPosition, float size = 50.0f) = 0;
    virtual void DrawTextureOnRadar(ITexture* texture, CVector2D radarPosition, CRGBA color, CVector2D size) = 0;

    virtual CVector2D ConvertWorldToScreenCoords(CVector worldPosition, bool useMenuCoords) = 0;
    virtual CVector2D ConvertMenuScreenCoordsToOS(CVector2D menuCoords) = 0;
    virtual CVector2D ConvertOS_ScreenCordsToMenu(CVector osCoords) = 0;

    virtual IAudio* GetOrLoadAudio(const std::string& audioFilePath) = 0;
    virtual IAudio* GetOrLoad3DAudio(const std::string& audioFilePath) = 0;

    virtual std::string GetLocalizationText(const std::string& key) = 0;

    virtual void LogOnScreen(const std::string& line, ScreenLogType type = ScreenLogType::Special, int displayTime = 5000) = 0;
    virtual void ShowBottomMessage(const std::string&, int duration = 5000) = 0;

    /*
    #define BEGIN_OPERATION(operation) LogHelper::BeginOperation(operation, "MenuSZK_" #operation, "")
    #define BEGIN_OPERATION_DESC(operation, description) LogHelper::BeginOperation(operation, "MenuSZK_" #operation, description)
    #define END_OPERATION(operation) LogHelper::EndOperation(operation)
    #define END_OPERATION_RESULT(operation, result) LogHelper::EndOperation(operation, result)
    */
    virtual void BeginOperation(unsigned int operationCode, const std::string& operationName, const std::string& description = "") = 0;
    virtual void EndOperation(unsigned int operationCode, const std::string& result = "") = 0;

    virtual void AddLogMessage(const std::string& message) = 0;
};