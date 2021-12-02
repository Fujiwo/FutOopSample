#include "Windows.h"
#include "Windowsx.h"
#include <cmath>
#include <string>
#include <vector>
#include <exception>
using namespace std;

namespace CadLib
{
    class Geo
    {
    public:
        static int Round(double value)
        {
            return static_cast<int>(::floor(value + 0.5));
        }

        template <class T>
        static void Sort(T& value1, T& value2)
        {
            if (value1 > value2)
                Swap(value1, value2);
        }

        template <class T>
        static void Swap(T& value1, T& value2)
        {
            auto temporary = value1   ;
            value1         = value2   ;
            value2         = temporary;
        }
    };

    class Point : public POINT
    {
    public:
        Point()
        {}

        Point(const POINT& point)
        {
            this->x = point.x;
            this->y = point.y;
        }

        Point(int x, int y)
        {
            this->x = x;
            this->y = y;
        }

        Point operator -(POINT another)
        {
            return Point(x - another.x, y - another.y);
        }

        int Absolute() const
        {
            return Geo::Round(::sqrt(x * x + y * y));
        }
    };

    struct Line
    {
        Point start, end;

        Line(Point start, Point end) : start(start), end(end)
        {}
    };

    class Figure
    {
    public:
        virtual void Draw(HDC hdc) = 0;
    };

    class Window
    {
        HWND handle;

    public:
        HWND GetHandle() const
        {
            return handle;
        }

        bool Create(HINSTANCE hInstance, const wchar_t* windowClassName, const wchar_t* title, UINT style, int showCommand)
        {
            handle = ::CreateWindow(windowClassName, title, style, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, this);
            if (handle == nullptr)
                return false;

            ::ShowWindow(handle, showCommand);
            ::UpdateWindow(handle);
            return true;
        }

        static ATOM RegisterWindowClass(HINSTANCE hInstance, const wchar_t* windowClassName)
        {
            WNDCLASSEXW wcex;
            ::ZeroMemory(&wcex, sizeof(WNDCLASSEX));

            wcex.cbSize = sizeof(WNDCLASSEX);
            if (::GetClassInfoEx(GetModuleHandle(nullptr), windowClassName, &wcex))
                return 0;

            wcex.style         = CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc   = WndProc;
            wcex.hInstance     = hInstance;
            wcex.hCursor       = ::LoadCursor(nullptr, IDC_ARROW);
            wcex.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
            wcex.lpszClassName = windowClassName;
            return ::RegisterClassEx(&wcex);
        }

    protected:
        virtual void OnCreate() {}
        virtual void OnDestroy() {}
        virtual void OnPaint(HDC hdc) {}
        virtual void OnLButtonUp(Point point) {}
        virtual void OnKeyDown(unsigned int key) {}

    private:
        // メッセージを処理
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message) {
            case WM_CREATE: {
                    auto createStruct = (CREATESTRUCT*)lParam;
                    auto self = reinterpret_cast<Window*>(createStruct->lpCreateParams);
                    self->handle = hWnd;
                    ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
                    self->OnCreate();
                }
                break;

            default: {
                    Window* self = GetSelf(hWnd);
                    return self == nullptr
                        ? ::DefWindowProc(hWnd, message, wParam, lParam)
                        : self->WindowProcedure(hWnd, message, wParam, lParam);
                }
                break;
            }
            return 0;
        }

        // メッセージを処理
        LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message) {
                case WM_PAINT: { // 描画時
                    PAINTSTRUCT ps;
                    HDC hdc = ::BeginPaint(hWnd, &ps);
                    OnPaint(hdc);
                    ::EndPaint(hWnd, &ps);
                }
                break;

            case WM_LBUTTONUP: // マウス左ボタンのアップ時
                OnLButtonUp(Point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
                break;

            case WM_KEYDOWN:// キーダウン時
                OnKeyDown((unsigned int)wParam);
                break;

            case WM_DESTROY: // ウィンドウの終了時
                OnDestroy();
                break;

            default:
                return ::DefWindowProc(hWnd, message, wParam, lParam);
            }
            return 0;
        }

        static Window* GetSelf(HWND hWnd)
        {
            return reinterpret_cast<Window*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }
    };

    class OverlappedWindow : public Window
    {
        static const wchar_t windowClassName[];

        HWND handle;

    public:
        bool Create(HINSTANCE hInstance, const wchar_t* title, int showCommand)
        {
            RegisterWindowClass(hInstance, windowClassName);
            return Window::Create(hInstance, windowClassName, title, WS_OVERLAPPEDWINDOW, showCommand);
        }
    };

    const wchar_t OverlappedWindow::windowClassName[] = L"FCOverlappedWindow";

    class Model
    {
        vector<Figure*> figures;

    public:
        virtual ~Model()
        {
            for (auto figure : figures)
                delete figure;
        }

        void Add(Figure* figure)
        {
            figures.push_back(figure);
        }

        vector<Figure*>::iterator begin()
        {
            return figures.begin();
        }

        vector<Figure*>::iterator end()
        {
            return figures.end();
        }
    };

    class Command
    {
        Model& model;

    protected:
        Model& GetModel()
        {
            return model;
        }

    public:
        Command(Model& model) : model(model)
        {}

        virtual void Draw(HDC hdc)
        {}

        virtual void OnInput(Point point)
        {}
    };

    class Application
    {
        const HINSTANCE   hInstance;
        const int         showCommand;
        wstring           title;
        OverlappedWindow& mainWindow;

    public:
        Application(HINSTANCE hInstance, OverlappedWindow& mainWindow, const wchar_t* title, int showCommand)
            : hInstance(hInstance), showCommand(showCommand), title(title), mainWindow(mainWindow)
        {}

        int Run()
        {
            return mainWindow.Create(hInstance, title.c_str(), showCommand) ? MainMessageLoop() : 0;
        }

    private:
        int MainMessageLoop() const
        {
            MSG msg;
            while (::GetMessage(&msg, nullptr, 0, 0)) {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
            return int(msg.wParam);
        }
    };
} // namespace CadLib

namespace MyCad
{
    using namespace CadLib;

    class LineFigure : public Figure
    {
        Point start, end;

    public:
        LineFigure(Point start, Point end) : start(start), end(end)
        {}

        virtual void Draw(HDC hdc) override
        {
            ::MoveToEx(hdc, start.x, start.y, nullptr);
            ::LineTo(hdc, end.x, end.y);
        }
    };

    class RectangleFigure : public Figure
    {
        RECT position;

    public:
        RectangleFigure(RECT position) : position(position)
        {
            Normalize();
        }

        virtual void Draw(HDC hdc) override
        {
            ::Rectangle(hdc, position.left, position.top, position.right, position.bottom);
        }

    private:
        void Normalize()
        {
            Geo::Sort(position.left, position.right);
            Geo::Sort(position.top, position.bottom);
        }
    };

    class CircleFigure : public Figure
    {
        Point center;
        int   radius;

    public:
        CircleFigure(Point center, int radius) : center(center), radius(radius)
        {
            if (radius <= 0)
                throw exception();
        }

        virtual void Draw(HDC hdc) override
        {
            ::Ellipse(hdc, center.x - radius, center.y - radius,
                center.x + radius, center.y + radius);
        }
    };

    class Point2Command : public Command
    {
        int   inputCount;
        Point firstPoint;

    public:
        Point2Command(Model& model) : Command(model), inputCount(0)
        {}

        virtual void Draw(HDC hdc)
        {
            switch (inputCount) {
            case 1:
                DrawPoint(hdc, firstPoint);
                break;
            }
        }

        virtual void OnInput(Point point) override
        {
            switch (inputCount) {
            case 0:
                firstPoint = point;
                inputCount = 1;
                break;
            case 1:
                OnDecide(firstPoint, Point(point));
                inputCount = 0;
                break;
            }
        }

    protected:
        virtual void OnDecide(Point point1, Point point2)
        {
            GetModel().Add(CreateFigure(point1, point2));
        }

        virtual Figure* CreateFigure(Point point1, Point point2) = 0;

    private:
        static void DrawPoint(HDC hdc, Point point)
        {
            const int size = 5;

            DrawLine(hdc, Line(Point(point.x, point.y - size), Point(point.x, point.y + size)));
            DrawLine(hdc, Line(Point(point.x - size, point.y), Point(point.x + size, point.y)));
        }

        static void DrawLine(HDC hdc, const Line& line)
        {
            ::MoveToEx(hdc, line.start.x, line.start.y, nullptr);
            ::LineTo  (hdc, line.end  .x, line.end  .y         );
        }
    };

    class AddCircleCommand : public Point2Command
    {
    public:
        AddCircleCommand(Model& model) : Point2Command(model)
        {}

    protected:
        virtual Figure* CreateFigure(Point point1, Point point2)
        {
            return new CircleFigure(point1, (point1 - point2).Absolute());
        }
    };

    class AddRectangleCommand : public Point2Command
    {
    public:
        AddRectangleCommand(Model& model) : Point2Command(model)
        {}

    protected:
        virtual Figure* CreateFigure(Point point1, Point point2)
        {
            const RECT position = { point1.x, point1.y, point2.x, point2.y };
            return new RectangleFigure(position);
        }
    };

    class AddLineCommand : public Point2Command
    {
    public:
        AddLineCommand(Model& model) : Point2Command(model)
        {}

    protected:
        virtual Figure* CreateFigure(Point point1, Point point2)
        {
            return new LineFigure(point1, point2);
        }
    };

    class MainWindow : public OverlappedWindow
    {
        Model&   model;
        Command* command;
        wstring  title;

    public:
        MainWindow(const wchar_t* title, Model& model) : title(title), model(model)
        {
            command = new AddCircleCommand(model);
        }

        virtual ~MainWindow()
        {
            delete command;
        }

    protected:
        virtual void OnPaint(HDC hdc) override
        {
            for (auto figure : model)
                figure->Draw(hdc);
            if (command != nullptr)
                command->Draw(hdc);
        }

        virtual void OnLButtonUp(Point point) override
        {
            command->OnInput(point);
            ::InvalidateRect(GetHandle(), nullptr, true);
        }

        virtual void OnKeyDown(unsigned int key) override
        {
            wstring commandName = L"";
            switch (key) {
                case 'L': commandName = L"Line"     ; ChangeCommand(new AddLineCommand     (model)); break;
                case 'C': commandName = L"Circle"   ; ChangeCommand(new AddCircleCommand   (model)); break;
                case 'R': commandName = L"Rectangle"; ChangeCommand(new AddRectangleCommand(model)); break;
            }
            ::SetWindowText(GetHandle(), (wstring(title) + L" - " + commandName).c_str());
        }

        virtual void OnDestroy() override 
        {
            ::PostQuitMessage(0);
        }

    private:
        void ChangeCommand(Command* command)
        {
            delete this->command;
            this->command = command;
        }
    };

    class CadApplication : public Application
    {
        static const wchar_t title[];

        Model      model;
        MainWindow mainWindow;

    public:
        CadApplication(HINSTANCE hInstance, int showCommand)
            : Application(hInstance, mainWindow, title, showCommand),
            mainWindow(title, model)
        {}
    };

    const wchar_t CadApplication::title[] = L"My CAD";
} // MyCad

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t* commandLine, int showCommand)
{
    return MyCad::CadApplication(hInstance, showCommand).Run();
}
