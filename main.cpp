#include <windows.h>
#include <math.h>
#include <algorithm>    // per std::min

// --- Configurazione colori -------------------------------------
COLORREF bgColor     = RGB(255, 255, 255);   // sfondo finestra
COLORREF circleColor = RGB( 64,  64,  64);   // bordo quadrante e numeri
COLORREF secColor    = RGB(255,   0,   0);   // lancetta secondi
COLORREF minColor    = RGB(  0, 128,   0);   // lancetta minuti
COLORREF hourColor   = RGB(  0,   0, 255);   // lancetta ore

// Funzione di utilità: converte gradi in radianti
double DegToRad(double deg) {
    return deg * 3.14159265358979323846 / 180.0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        // timer ogni secondo
        SetTimer(hwnd, 1, 1000, NULL);
        break;

    case WM_TIMER:
        // ridisegna
        InvalidateRect(hwnd, NULL, TRUE);
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // BACKGROUND
        RECT rc;
        GetClientRect(hwnd, &rc);
        HBRUSH hBrBg = CreateSolidBrush(bgColor);
        FillRect(hdc, &rc, hBrBg);
        DeleteObject(hBrBg);

        // centro e raggio
        int cx = (rc.right  - rc.left) / 2;
        int cy = (rc.bottom - rc.top)  / 2;
        int r  = std::min(cx, cy) - 10;  // margine 10px

        // CERCHIO
        HPEN penCircle = CreatePen(PS_SOLID, 2, circleColor);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        HPEN oldPen     = (HPEN)   SelectObject(hdc, penCircle);
        Ellipse(hdc, cx - r, cy - r, cx + r, cy + r);
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(penCircle);

        // NUMERI 1–12
        // font dimensione proporzionale
        int fontSize = r / 6;
        HFONT hFont = CreateFont(
            -fontSize, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
            TEXT("Arial")
        );
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, circleColor);

        double numRadius = r * 0.85;  // raggio interno per i numeri
        for (int i = 1; i <= 12; i++) {
            // prepara stringa
            TCHAR buf[3];
            wsprintf(buf, TEXT("%d"), i);

            // angolo in radianti, con -90° di offset
            double angle = DegToRad(i * 30.0 - 90.0);
            int tx = cx + (int)(numRadius * cos(angle));
            int ty = cy + (int)(numRadius * sin(angle));

            // riquadro di disegno centrato su (tx,ty)
            RECT trc = { tx - fontSize, ty - fontSize, tx + fontSize, ty + fontSize };
            DrawText(hdc, buf, -1, &trc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);

        // MANI DELL’OROLOGIO
        SYSTEMTIME st;
        GetLocalTime(&st);
        double secA  = DegToRad(st.wSecond  / 60.0 * 360.0 - 90);
        double minA  = DegToRad((st.wMinute + st.wSecond/60.0) / 60.0 * 360.0 - 90);
        double hourA = DegToRad(((st.wHour % 12) + st.wMinute/60.0) / 12.0 * 360.0 - 90);

        int xSec  = cx + (int)(r * 0.9  * cos(secA));
        int ySec  = cy + (int)(r * 0.9  * sin(secA));
        int xMin  = cx + (int)(r * 0.75 * cos(minA));
        int yMin  = cy + (int)(r * 0.75 * sin(minA));
        int xHour = cx + (int)(r * 0.5  * cos(hourA));
        int yHour = cy + (int)(r * 0.5  * sin(hourA));

        HPEN penHour = CreatePen(PS_SOLID, 5, hourColor);
        HPEN penMin  = CreatePen(PS_SOLID, 3, minColor);
        HPEN penSec  = CreatePen(PS_SOLID, 1, secColor);

        // ore
        oldPen = (HPEN)SelectObject(hdc, penHour);
        MoveToEx(hdc, cx, cy, NULL);
        LineTo(hdc, xHour, yHour);
        SelectObject(hdc, oldPen);

        // minuti
        oldPen = (HPEN)SelectObject(hdc, penMin);
        MoveToEx(hdc, cx, cy, NULL);
        LineTo(hdc, xMin, yMin);
        SelectObject(hdc, oldPen);

        // secondi
        oldPen = (HPEN)SelectObject(hdc, penSec);
        MoveToEx(hdc, cx, cy, NULL);
        LineTo(hdc, xSec, ySec);
        SelectObject(hdc, oldPen);

        DeleteObject(penHour);
        DeleteObject(penMin);
        DeleteObject(penSec);

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    WNDCLASSEX wc = { sizeof(wc) };
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = "WinTime24";

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Registrazione fallita!", "Errore", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Rimuove il pulsante "Maximizza"
    DWORD style = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX;

    HWND hwnd = CreateWindowEx(
        0, wc.lpszClassName, "WinTime24",
        style,
        CW_USEDEFAULT, CW_USEDEFAULT, 320, 340,
        NULL, NULL, hInstance, NULL
    );
    if (!hwnd) {
        MessageBox(NULL, "Creazione finestra fallita!", "Errore", MB_OK | MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

