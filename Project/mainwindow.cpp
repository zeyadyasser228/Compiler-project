#include <windows.h>
#include <windowsx.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cctype>

const int BTN_RUN_ANALYSIS = 1;
const int BTN_CLEAR_FIELDS = 2;


enum TinyTag {
    TAG_IF, TAG_THEN, TAG_ELSE, TAG_END,
    TAG_REPEAT, TAG_UNTIL, TAG_READ, TAG_WRITE,
    TAG_ID, TAG_NUMBER,
    TAG_ASSIGN, TAG_COMP, TAG_SEMI, TAG_PUNC, TAG_COMMA,
    TAG_PLUS, TAG_MINUS, TAG_MULT, TAG_DIV,
    TAG_UNKNOWN
};

struct TinyToken {
    int id;
    std::string category;
    std::string lexeme;

    TinyToken(int _id, std::string _cat, std::string _lex)
        : id(_id), category(_cat), lexeme(_lex) {}
};


std::string getTagName(TinyTag t) {
    switch (t) {
    case TAG_IF:     return "IF";
    case TAG_THEN:   return "THEN";
    case TAG_ELSE:   return "ELSE";
    case TAG_END:    return "END";
    case TAG_REPEAT: return "REPEAT";
    case TAG_UNTIL:  return "UNTIL";
    case TAG_READ:   return "READ";
    case TAG_WRITE:  return "WRITE";
    case TAG_ID:     return "IDENTIFIER";
    case TAG_NUMBER: return "NUMBER";
    case TAG_ASSIGN: return "ASSIGNMENT (:=)";
    case TAG_COMP:   return "COMPARISON";
    case TAG_SEMI:   return "SEMICOLON";
    case TAG_PUNC:   return "PUNCTUATION";
    case TAG_COMMA:  return "COMMA";
    case TAG_PLUS:   return "PLUS";
    case TAG_MINUS:  return "MINUS";
    case TAG_MULT:   return "MULT";
    case TAG_DIV:    return "DIV";
    default:         return "UNKNOWN";
    }
}

TinyTag findTag(const std::string& s) {
    if (s == "if")     return TAG_IF;
    if (s == "then")   return TAG_THEN;
    if (s == "else")   return TAG_ELSE;
    if (s == "end")    return TAG_END;
    if (s == "repeat") return TAG_REPEAT;
    if (s == "until")  return TAG_UNTIL;
    if (s == "read")   return TAG_READ;
    if (s == "write")  return TAG_WRITE;
    return TAG_ID;
}

std::vector<TinyToken> runScanner(const std::string& input) {
    std::vector<TinyToken> list;
    int i = 0, n = (int)input.size(), count = 1;

    while (i < n) {
        if (isspace(input[i])) { i++; continue; }

        if (input[i] == '{') {
            while (i < n && input[i] != '}') i++;
            if (i < n) i++;
            continue;
        }

        if (isalpha(input[i])) {
            std::string word;
            while (i < n && (isalnum(input[i]))) word += input[i++];
            std::string normalized = word;
            for (char& ch : normalized) {
                ch = (char)tolower((unsigned char)ch);
            }
            TinyTag tag = findTag(normalized);
            list.push_back(TinyToken(count++, getTagName(tag), word));
            continue;
        }

        if (isdigit(input[i])) {
            std::string num;
            while (i < n && isdigit(input[i])) num += input[i++];
            list.push_back(TinyToken(count++, "NUMBER", num));
            continue;
        }

        if (input[i] == ':') {
            if (i + 1 < n && input[i + 1] == '=') {
                list.push_back(TinyToken(count++, "ASSIGNMENT", ":="));
                i += 2;
            }
            else {
                list.push_back(TinyToken(count++, "UNKNOWN", ":"));
                i++;
            }
            continue;
        }
        if (input[i] == '<') {
            if (i + 1 < n && input[i + 1] == '=') {
                list.push_back(TinyToken(count++, "COMPARISON", "<="));
                i += 2;
            }
            else {
                list.push_back(TinyToken(count++, "COMPARISON", "<"));
                i++;
            }
            continue;
        }
        if (input[i] == '>') {
            if (i + 1 < n && input[i + 1] == '=') {
                list.push_back(TinyToken(count++, "COMPARISON", ">="));
                i += 2;
            }
            else {
                list.push_back(TinyToken(count++, "COMPARISON", ">"));
                i++;
            }
            continue;
        }

        switch (input[i]) {
        case '+': list.push_back(TinyToken(count++, "PLUS", "+")); break;
        case '-': list.push_back(TinyToken(count++, "MINUS", "-")); break;
        case '*': list.push_back(TinyToken(count++, "MULT", "*")); break;
        case '/': list.push_back(TinyToken(count++, "DIV", "/")); break;
        case '=': list.push_back(TinyToken(count++, "COMPARISON", "=")); break;
        case ';': list.push_back(TinyToken(count++, "SEMICOLON", ";")); break;
        case '(': list.push_back(TinyToken(count++, "PUNCTUATION", "(")); break;
        case ')': list.push_back(TinyToken(count++, "PUNCTUATION", ")")); break;
        case ',': list.push_back(TinyToken(count++, "COMMA", ",")); break;
        default:  list.push_back(TinyToken(count++, "UNKNOWN", std::string(1, input[i]))); break;
        }
        i++;
    }
    return list;
}


HWND hIn, hOut, hBtnRun, hBtnClear, hCredits, hCreditsTitle, hHeader, hInputLabel, hTokensLabel;
HFONT hFontTitle = NULL, hFontBody = NULL, hFontButton = NULL, hFontCredits = NULL, hFontOutput = NULL;
HBRUSH hBrushWindow = NULL, hBrushInput = NULL, hBrushOutput = NULL, hBrushCredits = NULL;
COLORREF kColorWindow = RGB(238, 244, 255);
COLORREF kColorInputBg = RGB(255, 255, 255);
COLORREF kColorOutputBg = RGB(250, 253, 255);
COLORREF kColorCreditsBg = RGB(226, 237, 255);
COLORREF kColorPrimary = RGB(37, 99, 235);
COLORREF kColorPrimaryHover = RGB(29, 78, 216);
COLORREF kColorSecondary = RGB(14, 116, 144);
COLORREF kColorSecondaryHover = RGB(8, 98, 122);
COLORREF kColorTitle = RGB(15, 23, 42);
COLORREF kColorText = RGB(30, 41, 59);
int gHoveredButtonId = 0;

void createUiResources() {
    hFontTitle = CreateFontA(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    hFontBody = CreateFontA(20, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    hFontButton = CreateFontA(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    hFontCredits = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    hFontOutput = CreateFontA(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FIXED_PITCH | FF_MODERN, "Consolas");

    hBrushWindow = CreateSolidBrush(kColorWindow);
    hBrushInput = CreateSolidBrush(kColorInputBg);
    hBrushOutput = CreateSolidBrush(kColorOutputBg);
    hBrushCredits = CreateSolidBrush(kColorCreditsBg);
}

void destroyUiResources() {
    if (hFontTitle) DeleteObject(hFontTitle);
    if (hFontBody) DeleteObject(hFontBody);
    if (hFontButton) DeleteObject(hFontButton);
    if (hFontCredits) DeleteObject(hFontCredits);
    if (hFontOutput) DeleteObject(hFontOutput);
    if (hBrushWindow) DeleteObject(hBrushWindow);
    if (hBrushInput) DeleteObject(hBrushInput);
    if (hBrushOutput) DeleteObject(hBrushOutput);
    if (hBrushCredits) DeleteObject(hBrushCredits);
}

void executeAnalysis() {
    int len = GetWindowTextLength(hIn);
    if (len == 0) return;

    std::string text(len + 1, '\0');
    GetWindowTextA(hIn, &text[0], len + 1);
    text.resize(len);

    std::vector<TinyToken> tokens = runScanner(text);

    std::ostringstream oss;
    oss << std::left
        << std::setw(10) << "Token#"
        << std::setw(16) << "Lexeme"
        << "Type\r\n";
    oss << "-------------------------------------------------------\r\n";
    for (const auto& t : tokens) {
        oss << std::left
            << std::setw(10) << t.id
            << std::setw(16) << t.lexeme
            << t.category << "\r\n";
    }
    SetWindowTextA(hOut, oss.str().c_str());
}

void clearFields() {
    SetWindowTextA(hIn, "");
    SetWindowTextA(hOut, "");
    SetFocus(hIn);
}

void layoutUi(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int clientW = rc.right - rc.left;
    int clientH = rc.bottom - rc.top;
    const int margin = 20;
    const int gap = 12;
    const int headerH = 40;
    const int labelH = 28;
    const int buttonH = 52;
    const int creditsTitleH = 28;
    const int creditsH = 96;
    const int minInputH = 150;
    const int minOutputH = 180;
    const int contentW = clientW - margin * 2;

    int y = 14;
    MoveWindow(hHeader, margin, y, contentW, headerH, TRUE);
    y += headerH + 6;

    MoveWindow(hInputLabel, margin, y, 240, labelH, TRUE);
    y += labelH + 4;

    int availableH = clientH - y - (buttonH + gap + labelH + 4 + creditsTitleH + 4 + creditsH + margin + gap * 3);
    int inputH = availableH / 2;
    if (inputH < minInputH) inputH = minInputH;
    int outputH = availableH - inputH;
    if (outputH < minOutputH) outputH = minOutputH;

    MoveWindow(hIn, margin, y, contentW, inputH, TRUE);
    y += inputH + gap;

    const int runW = 280;
    const int clearW = 190;
    const int btnGap = 16;
    int totalBtnW = runW + clearW + btnGap;
    int btnX = margin + (contentW - totalBtnW) / 2;
    MoveWindow(hBtnRun, btnX, y, runW, buttonH, TRUE);
    MoveWindow(hBtnClear, btnX + runW + btnGap, y, clearW, buttonH, TRUE);
    y += buttonH + gap;

    MoveWindow(hTokensLabel, margin, y, 220, labelH, TRUE);
    y += labelH + 4;

    MoveWindow(hOut, margin, y, contentW, outputH, TRUE);
    y += outputH + gap;

    const int creditsW = (contentW > 520) ? 520 : contentW;
    int creditsX = margin + (contentW - creditsW) / 2;
    MoveWindow(hCreditsTitle, creditsX, y, creditsW, creditsTitleH, TRUE);
    MoveWindow(hCredits, creditsX, y + creditsTitleH + 4, creditsW, creditsH, TRUE);
}

LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
    {
        createUiResources();

        hHeader = CreateWindowA("STATIC", "TINY Lexical Scanner", WS_VISIBLE | WS_CHILD, 20, 14, 420, 38, hwnd, NULL, NULL, NULL);
        SendMessageA(hHeader, WM_SETFONT, (WPARAM)hFontTitle, TRUE);

        hInputLabel = CreateWindowA("STATIC", "TINY Code Input:", WS_VISIBLE | WS_CHILD, 20, 66, 220, 26, hwnd, NULL, NULL, NULL);
        SendMessageA(hInputLabel, WM_SETFONT, (WPARAM)hFontBody, TRUE);

        hIn = CreateWindowA(
            "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL,
            20, 96, 940, 180, hwnd, NULL, NULL, NULL
        );
        SendMessageA(hIn, WM_SETFONT, (WPARAM)hFontBody, TRUE);

        hBtnRun = CreateWindowA(
            "BUTTON", "Run Lexical Analysis", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
            285, 292, 255, 50, hwnd, (HMENU)BTN_RUN_ANALYSIS, NULL, NULL
        );
        SendMessageA(hBtnRun, WM_SETFONT, (WPARAM)hFontButton, TRUE);

        hBtnClear = CreateWindowA(
            "BUTTON", "Clear Fields", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
            550, 292, 170, 50, hwnd, (HMENU)BTN_CLEAR_FIELDS, NULL, NULL
        );
        SendMessageA(hBtnClear, WM_SETFONT, (WPARAM)hFontButton, TRUE);

        hTokensLabel = CreateWindowA("STATIC", "Tokens Table:", WS_VISIBLE | WS_CHILD, 20, 344, 220, 26, hwnd, NULL, NULL, NULL);
        SendMessageA(hTokensLabel, WM_SETFONT, (WPARAM)hFontBody, TRUE);

        hOut = CreateWindowA(
            "EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_READONLY | ES_AUTOVSCROLL,
            20, 374, 940, 220, hwnd, NULL, NULL, NULL
        );
        SendMessageA(hOut, WM_SETFONT, (WPARAM)hFontOutput, TRUE);

        hCreditsTitle = CreateWindowA("STATIC", "Project Team", WS_VISIBLE | WS_CHILD | SS_CENTER, 390, 558, 330, 30, hwnd, NULL, NULL, NULL);
        SendMessageA(hCreditsTitle, WM_SETFONT, (WPARAM)hFontBody, TRUE);

        const char* creditsText =
            "Zeyad Yasser Salah   | Section 3\r\n"
            "Seif-Eldeen Taha     | Section 3\r\n"
            "Seif Mujahid         | Section 3\r\n"
            "Zeyad Magdy          | Section 3\r\n"
            "Ibrahim Mokhtar      | Section 1";

        hCredits = CreateWindowA(
            "EDIT", creditsText, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | ES_CENTER,
            390, 590, 330, 96, hwnd, NULL, NULL, NULL
        );
        SendMessageA(hCredits, WM_SETFONT, (WPARAM)hFontCredits, TRUE);
        layoutUi(hwnd);
        break;
    }
    case WM_COMMAND:
        if (LOWORD(wp) == BTN_RUN_ANALYSIS) executeAnalysis();
        if (LOWORD(wp) == BTN_CLEAR_FIELDS) clearFields();
        break;
    case WM_MOUSEMOVE:
    {
        POINT p = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
        RECT rcRun, rcClear;
        GetWindowRect(hBtnRun, &rcRun);
        GetWindowRect(hBtnClear, &rcClear);
        MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rcRun, 2);
        MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rcClear, 2);

        int hoverNow = 0;
        if (PtInRect(&rcRun, p)) hoverNow = BTN_RUN_ANALYSIS;
        if (PtInRect(&rcClear, p)) hoverNow = BTN_CLEAR_FIELDS;

        if (hoverNow != gHoveredButtonId) {
            gHoveredButtonId = hoverNow;
            InvalidateRect(hBtnRun, NULL, TRUE);
            InvalidateRect(hBtnClear, NULL, TRUE);
        }
        break;
    }
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lp;
        if (dis->CtlID == BTN_RUN_ANALYSIS || dis->CtlID == BTN_CLEAR_FIELDS) {
            const bool isRun = (dis->CtlID == BTN_RUN_ANALYSIS);
            COLORREF bg;
            if (isRun) {
                bg = (gHoveredButtonId == BTN_RUN_ANALYSIS) ? kColorPrimaryHover : kColorPrimary;
            }
            else {
                bg = (gHoveredButtonId == BTN_CLEAR_FIELDS) ? kColorSecondaryHover : kColorSecondary;
            }
            HBRUSH btnBrush = CreateSolidBrush(bg);
            FillRect(dis->hDC, &dis->rcItem, btnBrush);
            DeleteObject(btnBrush);

            SetBkMode(dis->hDC, TRANSPARENT);
            SetTextColor(dis->hDC, RGB(255, 255, 255));

            HFONT oldFont = (HFONT)SelectObject(dis->hDC, hFontButton);
            const char* caption = isRun ? "Run Lexical Analysis" : "Clear Fields";
            DrawTextA(dis->hDC, caption, -1, &dis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(dis->hDC, oldFont);
            return TRUE;
        }
        break;
    }
    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = (HDC)wp;
        HWND hCtl = (HWND)lp;
   
        if (hCtl == hOut) {
            SetBkMode(hdc, OPAQUE);
            SetTextColor(hdc, kColorText);
            SetBkColor(hdc, kColorOutputBg);
            return (LRESULT)hBrushOutput;
        }
        if (hCtl == hCredits) {
            SetBkMode(hdc, OPAQUE);
            SetTextColor(hdc, RGB(60, 77, 96));
            SetBkColor(hdc, kColorCreditsBg);
            return (LRESULT)hBrushCredits;
        }

        // Normal STATIC labels (titles/captions).
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, kColorTitle);
        return (LRESULT)hBrushWindow;
    }
    case WM_CTLCOLOREDIT:
    {
        HDC hdc = (HDC)wp;
        HWND hCtl = (HWND)lp;
        if (hCtl == hCredits) {
            SetTextColor(hdc, RGB(39, 60, 82));
            SetBkColor(hdc, kColorCreditsBg);
            return (LRESULT)hBrushCredits;
        }
        SetTextColor(hdc, kColorText);
        SetBkColor(hdc, (hCtl == hOut) ? kColorOutputBg : kColorInputBg);
        return (LRESULT)((hCtl == hOut) ? hBrushOutput : hBrushInput);
    }
    case WM_ERASEBKGND:
    {
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect((HDC)wp, &rc, hBrushWindow);
        return 1;
    }
    case WM_SIZE:
        layoutUi(hwnd);
        break;
    case WM_DESTROY:
        destroyUiResources();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcA(hwnd, msg, wp, lp);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    typedef BOOL(WINAPI* SetProcessDPIAwareFn)();
    HMODULE hUser32 = GetModuleHandleA("user32.dll");
    if (hUser32) {
        SetProcessDPIAwareFn setDpiAware =
            (SetProcessDPIAwareFn)GetProcAddress(hUser32, "SetProcessDPIAware");
        if (setDpiAware) setDpiAware();
    }

    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = MainProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "TinyAppClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA(
        "TinyAppClass",
        "TINY Scanner - Compiler Project",
        WS_OVERLAPPEDWINDOW,
        90, 70, 1000, 840,
        NULL, NULL, hInst, NULL
    );
    ShowWindow(hwnd, SW_MAXIMIZE);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}