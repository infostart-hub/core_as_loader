/*
* API файл для модуля "starter", создан 22.04.2021 12:22
* Препроцессор:
* console = 1
* ver = 1.0.0.0
* x64 = 0
* x86 = 1
*/

// Enums
enum InternalObjectsSizes {
	NoCaseSet_size = 40,
	NoCaseSetIterator_size = 8,
	UintMap_size = 40,
	UintMapIterator_size = 12,
	NoCaseMap_size = 48,
	NoCaseMapIterator_size = 12,
	utf8string_size = 4,
	StringComparator_size = 36,
	string_size = 4,
	Guid_size = 16,
	SourcesThunkFixup_size = 4,
	CppCatch_size = 12,
	TrapSwap_size = 20,
	TrapVirtualStdCall_size = 16,
	ScriptError_size = 28,
	Variant_size = 16,
};
enum StringCompareMethod {
	cmBegin = 0,
	cmBeginWithOtherLangs = 1,
	cmContain = 2,
};
enum VarEnums {
	VT_EMPTY = 0,
	VT_NULL = 1,
	VT_I2 = 2,
	VT_I4 = 3,
	VT_R4 = 4,
	VT_R8 = 5,
	VT_CY = 6,
	VT_DATE = 7,
	VT_BSTR = 8,
	VT_DISPATCH = 9,
	VT_ERROR = 10,
	VT_BOOL = 11,
	VT_VARIANT = 12,
	VT_UNKNOWN = 13,
	VT_DECIMAL = 14,
	VT_I1 = 16,
	VT_UI1 = 17,
	VT_UI2 = 18,
	VT_UI4 = 19,
	VT_I8 = 20,
	VT_UI8 = 21,
	VT_INT = 22,
	VT_UINT = 23,
	VT_VOID = 24,
	VT_HRESULT = 25,
	VT_PTR = 26,
	VT_SAFEARRAY = 27,
	VT_CARRAY = 28,
	VT_USERDEFINED = 29,
	VT_LPSTR = 30,
	VT_LPWSTR = 31,
	VT_RECORD = 36,
	VT_INT_PTR = 37,
	VT_UINT_PTR = 38,
	VT_FILETIME = 64,
	VT_BLOB = 65,
	VT_STREAM = 66,
	VT_STORAGE = 67,
	VT_STREAMED_OBJECT = 68,
	VT_STORED_OBJECT = 69,
	VT_BLOB_OBJECT = 70,
	VT_CF = 71,
	VT_CLSID = 72,
	VT_VERSIONED_STREAM = 73,
	VT_BSTR_BLOB = 4095,
	VT_VECTOR = 4096,
	VT_ARRAY = 8192,
	VT_BYREF = 16384,
	VT_RESERVED = 32768,
	VT_ILLEGAL = 65535,
	VT_ILLEGALMASKED = 4095,
	VT_TYPEMASK = 4095,
	VARIANT_TRUE = -1,
	VARIANT_FALSE = 0,
};
enum CallTypes {
	asCALL_CDECL = 0,
	asCALL_STDCALL = 1,
	asCALL_THISCALL = 3,
};
enum TrapState {
	trapNotActive = 0,
	trapDisabled = 1,
	trapEnabled = 2,
};
enum starter__someConstValues {
	sizeof_ptr = 4,
	TYPEATTR_size = 76,
	TYPEATTR_guid_offset = 0,
	TYPEATTR_lcid_offset = 16,
	TYPEATTR_dwReserved_offset = 20,
	TYPEATTR_memidConstructor_offset = 24,
	TYPEATTR_memidDestructor_offset = 28,
	TYPEATTR_lpstrSchema_offset = 32,
	TYPEATTR_cbSizeInstance_offset = 36,
	TYPEATTR_typekind_offset = 40,
	TYPEATTR_cFuncs_offset = 44,
	TYPEATTR_cVars_offset = 46,
	TYPEATTR_cImplTypes_offset = 48,
	TYPEATTR_cbSizeVft_offset = 50,
	TYPEATTR_cbAlignment_offset = 52,
	TYPEATTR_wTypeFlags_offset = 54,
	TYPEATTR_wMajorVerNum_offset = 56,
	TYPEATTR_wMinorVerNum_offset = 58,
	TYPEATTR_tdescAlias_offset = 60,
	TYPEATTR_idldescType_offset = 68,
	FUNCDESC_size = 64,
	FUNCDESC_memid_offset = 0,
	FUNCDESC_lprgscode_offset = 4,
	FUNCDESC_lprgelemdescParam_offset = 8,
	FUNCDESC_funckind_offset = 12,
	FUNCDESC_invkind_offset = 16,
	FUNCDESC_callconv_offset = 20,
	FUNCDESC_cParams_offset = 24,
	FUNCDESC_cParamsOpt_offset = 26,
	FUNCDESC_oVft_offset = 28,
	FUNCDESC_cScodes_offset = 30,
	SqliteTransactionGuard_size = 8,
	SqliteTransactionGuard_db_offset = 0,
	SqliteTransactionGuard_success_offset = 4,
	LOGFONT_size = 92,
	LOGFONT_lfHeight_offset = 0,
	LOGFONT_lfWidth_offset = 4,
	LOGFONT_lfEscapement_offset = 8,
	LOGFONT_lfOrientation_offset = 12,
	LOGFONT_lfWeight_offset = 16,
	LOGFONT_lfItalic_offset = 20,
	LOGFONT_lfUnderline_offset = 21,
	LOGFONT_lfStrikeOut_offset = 22,
	LOGFONT_lfCharSet_offset = 23,
	LOGFONT_lfOutPrecision_offset = 24,
	LOGFONT_lfClipPrecision_offset = 25,
	LOGFONT_lfQuality_offset = 26,
	LOGFONT_lfPitchAndFamily_offset = 27,
	LOGFONT_lfFaceNameStart_offset = 28,
	LOGFONT_lfFaceNameEnd_offset = 90,
	MONITORINFO_size = 40,
	MONITORINFO_cbSize_offset = 0,
	MONITORINFO_rcMonitor_offset = 4,
	MONITORINFO_rcWork_offset = 20,
	MONITORINFO_dwFlags_offset = 36,
	MSG_size = 28,
	MSG_hwnd_offset = 0,
	MSG_message_offset = 4,
	MSG_wParam_offset = 8,
	MSG_lParam_offset = 12,
	MSG_time_offset = 16,
	MSG_pt_offset = 20,
	Rect_size = 16,
	Rect_left_offset = 0,
	Rect_top_offset = 4,
	Rect_right_offset = 8,
	Rect_bottom_offset = 12,
	Point_size = 8,
	Point_x_offset = 0,
	Point_y_offset = 4,
	Size_size = 8,
	Size_cx_offset = 0,
	Size_cy_offset = 4,
	MINMAXINFO_size = 40,
	MINMAXINFO_ptReserved_offset = 0,
	MINMAXINFO_ptMaxSize_offset = 8,
	MINMAXINFO_ptMaxPosition_offset = 16,
	MINMAXINFO_ptMinTrackSize_offset = 24,
	MINMAXINFO_ptMaxTrackSize_offset = 32,
	FILETIME_size = 8,
	FILETIME_dwLowDateTime_offset = 0,
	FILETIME_dwHighDateTime_offset = 4,
	WIN32_FIND_DATA_size = 648,
	WIN32_FIND_DATA_dwFileAttributes_offset = 0,
	WIN32_FIND_DATA_ftCreationTime_offset = 4,
	WIN32_FIND_DATA_ftLastAccessTime_offset = 12,
	WIN32_FIND_DATA_ftLastWriteTime_offset = 20,
	WIN32_FIND_DATA_nFileSizeHigh_offset = 28,
	WIN32_FIND_DATA_nFileSizeLow_offset = 32,
	WIN32_FIND_DATA_dwReserved0_offset = 36,
	WIN32_FIND_DATA_dwReserved1_offset = 40,
	WIN32_FIND_DATA_cFileName_offset = 44,
};
enum TYPEKIND {
	TKIND_ENUM = 0,
	TKIND_RECORD = 1,
	TKIND_MODULE = 2,
	TKIND_INTERFACE = 3,
	TKIND_DISPATCH = 4,
};
enum FUNCKIND {
	FUNC_VIRTUAL = 0,
	FUNC_PUREVIRTUAL = 1,
	FUNC_NONVIRTUAL = 2,
	FUNC_STATIC = 3,
	FUNC_DISPATCH = 4,
};
enum INVOKEKIND {
	INVOKE_FUNC = 1,
	INVOKE_PROPERTYGET = 2,
	INVOKE_PROPERTYPUT = 4,
};
enum s3const {
	SQLITE_OK = 0,
	SQLITE_ERROR = 1,
	SQLITE_INTERNAL = 2,
	SQLITE_PERM = 3,
	SQLITE_ABORT = 4,
	SQLITE_BUSY = 5,
	SQLITE_LOCKED = 6,
	SQLITE_NOMEM = 7,
	SQLITE_READONLY = 8,
	SQLITE_INTERRUPT = 9,
	SQLITE_IOERR = 10,
	SQLITE_CORRUPT = 11,
	SQLITE_NOTFOUND = 12,
	SQLITE_FULL = 13,
	SQLITE_CANTOPEN = 14,
	SQLITE_PROTOCOL = 15,
	SQLITE_EMPTY = 16,
	SQLITE_SCHEMA = 17,
	SQLITE_TOOBIG = 18,
	SQLITE_CONSTRAINT = 19,
	SQLITE_MISMATCH = 20,
	SQLITE_MISUSE = 21,
	SQLITE_NOLFS = 22,
	SQLITE_AUTH = 23,
	SQLITE_FORMAT = 24,
	SQLITE_RANGE = 25,
	SQLITE_NOTADB = 26,
	SQLITE_NOTICE = 27,
	SQLITE_WARNING = 28,
	SQLITE_ROW = 100,
	SQLITE_DONE = 101,
	SQLITE_INTEGER = 1,
	SQLITE_FLOAT = 2,
	SQLITE_TEXT = 3,
	SQLITE_BLOB = 4,
	SQLITE_NULL = 5,
	SQLITE_STATIC = 0,
	SQLITE_TRANSIENT = -1,
	SQLITE_OPEN_READONLY = 1,
	SQLITE_OPEN_READWRITE = 2,
	SQLITE_OPEN_CREATE = 4,
	SQLITE_OPEN_DELETEONCLOSE = 8,
	SQLITE_OPEN_EXCLUSIVE = 16,
	SQLITE_OPEN_AUTOPROXY = 32,
	SQLITE_OPEN_URI = 64,
	SQLITE_OPEN_MEMORY = 128,
	SQLITE_OPEN_MAIN_DB = 256,
	SQLITE_OPEN_TEMP_DB = 512,
	SQLITE_OPEN_TRANSIENT_DB = 1024,
	SQLITE_OPEN_MAIN_JOURNAL = 2048,
	SQLITE_OPEN_TEMP_JOURNAL = 4096,
	SQLITE_OPEN_SUBJOURNAL = 8192,
	SQLITE_OPEN_MASTER_JOURNAL = 16384,
	SQLITE_OPEN_NOMUTEX = 32768,
	SQLITE_OPEN_FULLMUTEX = 65536,
	SQLITE_OPEN_SHAREDCACHE = 131072,
	SQLITE_OPEN_PRIVATECACHE = 262144,
	SQLITE_OPEN_WAL = 524288,
};
enum WndMessages {
	WM_DESTROY = 2,
	WM_MOVE = 3,
	WM_SIZE = 5,
	WM_SETFOCUS = 7,
	WM_KILLFOCUS = 8,
	WM_PAINT = 15,
	WM_CLOSE = 16,
	WM_GETMINMAXINFO = 36,
	WM_SETFONT = 48,
	WM_NOTIFY = 78,
	WM_NCCALCSIZE = 131,
	WM_KEYDOWN = 256,
	WM_KEYUP = 257,
	WM_CHAR = 258,
	WM_DEADCHAR = 259,
	WM_SYSKEYDOWN = 260,
	WM_SYSKEYUP = 261,
	WM_SYSDEADCHAR = 263,
	WM_UNICHAR = 265,
	WM_INITDIALOG = 272,
	WM_COMMAND = 273,
	WM_LBUTTONDOWN = 513,
	WM_LBUTTONUP = 514,
	WM_RBUTTONDOWN = 516,
	WM_RBUTTONUP = 517,
	WM_SIZING = 532,
	EN_CHANGE = 768,
	WM_HOTKEY = 786,
	WM_CONTEXTMENU = 123,
};
enum WinConstants {
	DEFAULT_GUI_FONT = 17,
};
enum VirtualKeyCodes {
	VK_LBUTTON = 1,
	VK_RBUTTON = 2,
	VK_CANCEL = 3,
	VK_MBUTTON = 4,
	VK_XBUTTON1 = 5,
	VK_XBUTTON2 = 6,
	VK_BACK = 8,
	VK_TAB = 9,
	VK_CLEAR = 12,
	VK_RETURN = 13,
	VK_SHIFT = 16,
	VK_CONTROL = 17,
	VK_MENU = 18,
	VK_PAUSE = 19,
	VK_CAPITAL = 20,
	VK_KANA = 21,
	VK_HANGEUL = 21,
	VK_HANGUL = 21,
	VK_JUNJA = 23,
	VK_FINAL = 24,
	VK_HANJA = 25,
	VK_KANJI = 25,
	VK_ESCAPE = 27,
	VK_CONVERT = 28,
	VK_NONCONVERT = 29,
	VK_ACCEPT = 30,
	VK_MODECHANGE = 31,
	VK_SPACE = 32,
	VK_PRIOR = 33,
	VK_NEXT = 34,
	VK_END = 35,
	VK_HOME = 36,
	VK_LEFT = 37,
	VK_UP = 38,
	VK_RIGHT = 39,
	VK_DOWN = 40,
	VK_SELECT = 41,
	VK_PRINT = 42,
	VK_EXECUTE = 43,
	VK_SNAPSHOT = 44,
	VK_INSERT = 45,
	VK_DELETE = 46,
	VK_HELP = 47,
	VK_LWIN = 91,
	VK_RWIN = 92,
	VK_APPS = 93,
	VK_SLEEP = 95,
	VK_NUMPAD0 = 96,
	VK_NUMPAD1 = 97,
	VK_NUMPAD2 = 98,
	VK_NUMPAD3 = 99,
	VK_NUMPAD4 = 100,
	VK_NUMPAD5 = 101,
	VK_NUMPAD6 = 102,
	VK_NUMPAD7 = 103,
	VK_NUMPAD8 = 104,
	VK_NUMPAD9 = 105,
	VK_MULTIPLY = 106,
	VK_ADD = 107,
	VK_SEPARATOR = 108,
	VK_SUBTRACT = 109,
	VK_DECIMAL = 110,
	VK_DIVIDE = 111,
	VK_F1 = 112,
	VK_F2 = 113,
	VK_F3 = 114,
	VK_F4 = 115,
	VK_F5 = 116,
	VK_F6 = 117,
	VK_F7 = 118,
	VK_F8 = 119,
	VK_F9 = 120,
	VK_F10 = 121,
	VK_F11 = 122,
	VK_F12 = 123,
	VK_F13 = 124,
	VK_F14 = 125,
	VK_F15 = 126,
	VK_F16 = 127,
	VK_F17 = 128,
	VK_F18 = 129,
	VK_F19 = 130,
	VK_F20 = 131,
	VK_F21 = 132,
	VK_F22 = 133,
	VK_F23 = 134,
	VK_F24 = 135,
	VK_NUMLOCK = 144,
	VK_SCROLL = 145,
	VK_OEM_NEC_EQUAL = 146,
	VK_OEM_FJ_JISHO = 146,
	VK_OEM_FJ_MASSHOU = 147,
	VK_OEM_FJ_TOUROKU = 148,
	VK_OEM_FJ_LOYA = 149,
	VK_OEM_FJ_ROYA = 150,
	VK_LSHIFT = 160,
	VK_RSHIFT = 161,
	VK_LCONTROL = 162,
	VK_RCONTROL = 163,
	VK_LMENU = 164,
	VK_RMENU = 165,
	VK_BROWSER_BACK = 166,
	VK_BROWSER_FORWARD = 167,
	VK_BROWSER_REFRESH = 168,
	VK_BROWSER_STOP = 169,
	VK_BROWSER_SEARCH = 170,
	VK_BROWSER_FAVORITES = 171,
	VK_BROWSER_HOME = 172,
	VK_VOLUME_MUTE = 173,
	VK_VOLUME_DOWN = 174,
	VK_VOLUME_UP = 175,
	VK_MEDIA_NEXT_TRACK = 176,
	VK_MEDIA_PREV_TRACK = 177,
	VK_MEDIA_STOP = 178,
	VK_MEDIA_PLAY_PAUSE = 179,
	VK_LAUNCH_MAIL = 180,
	VK_LAUNCH_MEDIA_SELECT = 181,
	VK_LAUNCH_APP1 = 182,
	VK_LAUNCH_APP2 = 183,
	VK_OEM_1 = 186,
	VK_OEM_PLUS = 187,
	VK_OEM_COMMA = 188,
	VK_OEM_MINUS = 189,
	VK_OEM_PERIOD = 190,
	VK_OEM_2 = 191,
	VK_OEM_3 = 192,
	VK_OEM_4 = 219,
	VK_OEM_5 = 220,
	VK_OEM_6 = 221,
	VK_OEM_7 = 222,
	VK_OEM_8 = 223,
	VK_PROCESSKEY = 229,
	VK_ICO_CLEAR = 230,
	VK_PACKET = 231,
	VK_OEM_RESET = 233,
	VK_OEM_JUMP = 234,
	VK_OEM_PA1 = 235,
	VK_OEM_PA2 = 236,
	VK_OEM_PA3 = 237,
	VK_OEM_WSCTRL = 238,
	VK_OEM_CUSEL = 239,
	VK_OEM_ATTN = 240,
	VK_OEM_FINISH = 241,
	VK_OEM_COPY = 242,
	VK_OEM_AUTO = 243,
	VK_OEM_ENLW = 244,
	VK_OEM_BACKTAB = 245,
	VK_ATTN = 246,
	VK_CRSEL = 247,
	VK_EXSEL = 248,
	VK_EREOF = 249,
	VK_PLAY = 250,
	VK_ZOOM = 251,
	VK_NONAME = 252,
	VK_PA1 = 253,
	VK_OEM_CLEAR = 254,
};
enum FileAttributes {
	FILE_ATTRIBUTE_DIRECTORY = 16,
};

// Funcdefs
typedef bool array::(*less)(const T&, const T&);
typedef string (*RegExpReplaceFunc)(RegExpResult&&);
typedef uint (*WndFunc)(uint, uint, uint);

// Typedefs
typedef uint int_ptr;
typedef uint size_t;
typedef uint HWND;
typedef uint HANDLE;
typedef uint HDC;
typedef uint COLORREF;
typedef uint HICON;
typedef uint HMENU;
typedef uint UINT;
typedef int BOOL;
typedef uint8 BYTE;
typedef uint PTR;
typedef uint DWORD;
typedef uint HFONT;
typedef uint HGDIOBJ;
typedef uint HMONITOR;
typedef uint WPARAM;
typedef uint LPARAM;
typedef uint LRESULT;
typedef int LONG;
typedef int long;

// Objects
template <class T>
class array {
public:
	T& operator[](uint index);
	const T& operator[](uint index) const;
	array<T>& operator=(const array<T>&);
	void insertAt(uint index, const T& value);
	void insertAt(uint index, const array<T>& arr);
	void insertLast(const T& value);
	void removeAt(uint index);
	void removeLast();
	void removeRange(uint start, uint count);
	void reserve(uint length);
	void resize(uint length);
	void sortAsc();
	void sortAsc(uint startAt, uint count);
	void sortDesc();
	void sortDesc(uint startAt, uint count);
	void reverse();
	int find(const T& value) const;
	int find(uint startAt, const T& value) const;
	int findByRef(const T& value) const;
	int findByRef(uint startAt, const T& value) const;
	bool operator==(const array<T>&) const;
	bool isEmpty() const;
	void sort(array::less&, uint startAt = 0, uint count = uint ( - 1 ));
	uint length;
	void set_length(uint);
};

template <class T>
class grid {
public:
	T& operator[](uint, uint);
	const T& operator[](uint, uint) const;
	void resize(uint width, uint height);
	uint width() const;
	uint height() const;
};

class any {
public:
	any& operator=(any&);
	void store(?&);
	void store(const int64&);
	void store(const double&);
	bool retrieve(?&);
	bool retrieve(int64&);
	bool retrieve(double&);
};

class RegExp {
public:
	int find(const string&, uint offset = 0, uint& len);
	uint countOf(const string&, uint maxCount = 0, uint offset = 0);
	RegExpResult&& match(const string&, uint maxCount = 0, uint offset = 0);
	string& replace(string&, const string& repl, uint maxCount = 0, uint offset = 0);
	string& replace(string&, RegExpReplaceFunc&& cb, uint maxCount = 0, uint offset = 0);
	array<string>&& split(const string&) const;
	string extract(const string& text) const;
	string& remove(string& text) const;
	const bool isValid;	// 0xC (12)
	const string errorString;	// 0x8 (8)
};

class RegExpResult {
public:
	uint begin(uint match, uint group) const;
	uint len(uint match, uint group) const;
	const string& text(uint match, uint group) const;
	const uint matches;	// 0x4 (4)
	const uint groups;	// 0x8 (8)
};

class NoCaseSet {
public:
	~NoCaseSet();
	NoCaseSet();
	uint count() const;
	bool contains(const string& key) const;
	bool insert(const string& key);
	bool remove(const string& key);
	void clear();
	void swap(NoCaseSet& other);
	NoCaseSetIterator begin() const;
};

class NoCaseSetIterator {
public:
	~NoCaseSetIterator();
	NoCaseSetIterator();
	NoCaseSetIterator(const NoCaseSetIterator& other);
	NoCaseSetIterator& operator=(const NoCaseSetIterator& other);
	bool next();
	bool operator++(int);
	const string& key;
};

template <class T>
class UintMap {
public:
	~UintMap();
	UintMap(int&);
	uint count() const;
	bool contains(uint key) const;
	bool insert(uint key, const T& val);
	bool tryInsert(uint key, const T& val);
	bool remove(uint key);
	void clear();
	void swap(UintMap<T>& other);
	UintMapIterator<T> begin() const;
	UintMapIterator<T> find(uint key) const;
};

template <class T>
class UintMapIterator {
public:
	~UintMapIterator();
	UintMapIterator(int&);
	UintMapIterator<T>& operator=(const UintMapIterator<T>&);
	bool next();
	bool operator++(int);
	bool isEnd() const;
	uint key;
	T& value;
};

template <class T>
class NoCaseMap {
public:
	~NoCaseMap();
	NoCaseMap(int&);
	uint count() const;
	bool contains(const string& key) const;
	bool insert(const string& key, const T& val);
	bool tryInsert(const string& key, const T& val);
	bool remove(const string& key);
	void clear();
	void swap(NoCaseMap<T>& other);
	NoCaseMapIterator<T> begin() const;
	NoCaseMapIterator<T> find(const string& key) const;
};

template <class T>
class NoCaseMapIterator {
public:
	~NoCaseMapIterator();
	NoCaseMapIterator(int&);
	NoCaseMapIterator<T>& operator=(const NoCaseMapIterator<T>&);
	bool next();
	bool operator++(int);
	bool isEnd() const;
	const string& key;
	T& value;
};

class utf8string {
public:
	~utf8string();
	utf8string();
	utf8string(uint text, uint len = - 1);
	utf8string(const utf8string& other);
	uint length;
	operator uint() const;
	operator string() const;
	string str;
	uint ptr;	// 0x0 (0)
};

class StringComparator {
public:
	~StringComparator();
	StringComparator();
	void setPattern(const string& str, StringCompareMethod cm);
	bool match(const string& str) const;
};

class string {
public:
	~string();
	string();
	string(const char*);
	string(const string& other);
	string(const uint16& rawPtr);
	string(uint addr, uint len);
	string(const string& pattern, uint repeat);
	string(uint num);
	string(int num);
	string(uint64 num);
	string(int64 num);
	string(double num);
	string(bool val);
	int find(RegExp&&, uint offset = 0, uint& len) const;
	uint countOf(RegExp&&, uint maxCount = 0, uint offset = 0) const;
	RegExpResult&& match(RegExp&&, uint maxCount = 0, uint offset = 0) const;
	string& replace(RegExp&&, const string& repl, uint maxCount = 0, uint offset = 0);
	string& replace(RegExp&&, RegExpReplaceFunc&& cb, uint maxCount = 0, uint offset = 0);
	array<string>&& split(RegExp&&) const;
	string extract(RegExp&&) const;
	string& remove(RegExp&&);
	void ctor(uint addr, uint len);
	string& operator=(const string& other);
	string& operator=(uint num);
	string& operator=(int num);
	string& operator=(uint64 num);
	string& operator=(int64 num);
	string& operator=(double num);
	string& operator=(bool val);
	string operator+(const string& other) const;
	string operator+(uint num) const;
	string operator+(int num) const;
	string operator+(uint64 num) const;
	string operator+(int64 num) const;
	string operator+(double num) const;
	string operator+(bool val) const;
	string& operator+=(const string& other);
	string& operator+=(uint num);
	string& operator+=(int num);
	string& operator+=(uint64 num);
	string& operator+=(int64 num);
	string& operator+=(double num);
	string& operator+=(bool val);
	int operator<(const string& other) const;
	void empty();
	string& trim();
	string& ltrim();
	string& rtrim();
	string& trim(const string& symbols);
	string& ltrim(const string& symbols);
	string& rtrim(const string& symbols);
	string trimmed() const;
	string ltrimmed() const;
	string rtrimmed() const;
	uint find(const string& pattern, uint offset = 0) const;
	uint find(uint16 symb, uint offset = 0) const;
	uint length;
	bool isEmpty() const;
	array<string>&& split(const string& delimiter) const;
	string& replace(const string& pattern, const string& repl);
	string& replace(uint16 from, uint16 to);
	string replaced(const string& pattern, const string& repl) const;
	string& makeUpper();
	string uppered() const;
	string& makeLower();
	string lowered() const;
	string& insert(uint pos, const string& text);
	string& insert(uint pos, uint16 symbol);
	string& remove(uint from, uint count = 1);
	string& replace(uint pos, uint16 symbol);
	string substr(int start, int len = 0) const;
	string mid(uint start, int len = - 1) const;
	string dup() const;
	int compareNoCase(const string& other) const;
	uint cstr;
	uint16 operator[](uint pos) const;
	uint setLength(uint);
	string& padRight(uint16 symbol, uint width);
	string& padLeft(uint16 symbol, uint width);
	bool beginFrom(const string& other) const;
	utf8string toUtf8() const;
	string& fromUtf8(const utf8string& other);
	uint self;
};

class Guid {
public:
	Guid(const string& str);
	bool parseString(const string& str);
	Guid& operator=(const string&);
	operator string() const;
	string str;
	bool operator==(const Guid&) const;
	GuidRef&& ref() const;
	uint self;
	uint data1;	// 0x0 (0)
	uint data2;	// 0x4 (4)
	uint data3;	// 0x6 (6)
	uint data4;	// 0x8 (8)
};

class SourcesThunkFixup {
public:
	~SourcesThunkFixup();
	SourcesThunkFixup();
};

class CppCatch {
public:
	~CppCatch();
	CppCatch(const string& typeName, ?& handler);
};

class TrapSwap {
public:
	~TrapSwap();
	TrapSwap();
	void setTrapByName(const string& module, const string& proc, int callType, ?&);
	void swap();
	void getOriginal(?&);
	const TrapState state;	// 0x10 (16)
};

class TrapVirtualStdCall {
public:
	~TrapVirtualStdCall();
	TrapVirtualStdCall();
	void setTrap(?& obj, uint vtIdx, ?& func, int callType = asCALL_STDCALL);
	void swap();
	void getOriginal(?&);
	const TrapState state;	// 0xC (12)
};

class ScriptError {
public:
	uint line;	// 0x0 (0)
	uint col;	// 0x4 (4)
	uint errCode;	// 0x8 (8)
	string source;	// 0xC (12)
	string description;	// 0x10 (16)
	string sourceCode;	// 0x14 (20)
	bool bDebugPossible;	// 0x18 (24)
};

class as_env {
public:
	const string& option(const string& key) const;
	bool optionsContainAndEqual(const string& key, const string& test) const;
	bool optionsContainAndNotEqual(const string& key, const string& test) const;
	uint64 optionAsNumber(const string& key, uint64 defVal) const;
	const string& dataDir;
	const string& moduleDir;
	const string& tempDir;
	const string& logDir;
	const string& dumpDir;
	string currentDir;
	void set_currentDir(const string& dir) const;
	string getEnvVar(const string& varName) const;
	uint logLevel;
	void showConsole() const;
	uint64 intParam(const string& name) const;
	const string& strParam(const string& name) const;
	const string mainFolder;	// 0x0 (0)
	const string processName;	// 0x4 (4)
	uint64 processVersion;	// 0x8 (8)
	uint64 coreAsVersion;	// 0x10 (16)
	const string strProcessVersion;	// 0x18 (24)
	const string strCoreAsVersion;	// 0x1C (28)
	const string ownerName;	// 0x20 (32)
};

class IUnknown {
public:
	uint AddRef();
	uint Release();
	uint queryIface(const Guid&, IUnknown&&&);
	uint self;
	IUnknown& unk;
	operator IDispatch&&();
	operator IConnectionPointContainer&&();
};

class IDispatch {
public:
	uint AddRef();
	uint Release();
	uint queryIface(const Guid&, IUnknown&&&);
	operator IUnknown&&();
	uint self;
	IUnknown& unk;
	bool findMember(const string& name, int& id);
	bool getParamsCount(int id, int& params);
	bool call(int id, array<Variant>& args, Variant& res);
	bool getProp(const string& name, Variant& res);
	bool setProp(const string& name, const Variant& newVal);
};

class IConnectionPointContainer {
public:
	uint AddRef();
	uint Release();
	uint queryIface(const Guid&, IUnknown&&&);
	operator IUnknown&&();
	uint self;
	IUnknown& unk;
	uint FindConnectionPoint(const Guid& riid, IUnknown&&&);
};

class TYPEATTR {
public:
	TYPEATTRRef&& ref() const;
	uint self;
	Guid guid;	// 0x0 (0)
	uint lcid;	// 0x10 (16)
	uint dwReserved;	// 0x14 (20)
	int memidConstructor;	// 0x18 (24)
	int memidDestructor;	// 0x1C (28)
	uint lpstrSchema;	// 0x20 (32)
	uint cbSizeInstance;	// 0x24 (36)
	uint typekind;	// 0x28 (40)
	uint16 cFuncs;	// 0x2C (44)
	uint16 cVars;	// 0x2E (46)
	uint16 cImplTypes;	// 0x30 (48)
	uint16 cbSizeVft;	// 0x32 (50)
	uint16 cbAlignment;	// 0x34 (52)
	uint16 wTypeFlags;	// 0x36 (54)
	uint16 wMajorVerNum;	// 0x38 (56)
	uint16 wMinorVerNum;	// 0x3A (58)
	uint64 tdescAlias;	// 0x3C (60)
	uint64 idldescType;	// 0x44 (68)
};

class FUNCDESC {
public:
	FUNCDESCRef&& ref() const;
	uint self;
	uint memid;	// 0x0 (0)
	uint lprgscode;	// 0x4 (4)
	uint lprgelemdescParam;	// 0x8 (8)
	uint funckind;	// 0xC (12)
	uint invkind;	// 0x10 (16)
	uint callconv;	// 0x14 (20)
	uint16 cParams;	// 0x18 (24)
	uint16 cParamsOpt;	// 0x1A (26)
	uint16 oVft;	// 0x1C (28)
	uint16 cScodes;	// 0x1E (30)
};

class SqliteTransactionGuard {
public:
	~SqliteTransactionGuard();
	SqliteTransactionGuard(uint _db);
	SqliteTransactionGuardRef&& ref() const;
	uint self;
	void ctor(uint _db);
	void dtor();
	uint db;	// 0x0 (0)
	bool success;	// 0x4 (4)
};

class LOGFONT {
public:
	LOGFONTRef&& ref() const;
	uint self;
	int lfHeight;	// 0x0 (0)
	int lfWidth;	// 0x4 (4)
	int lfEscapement;	// 0x8 (8)
	int lfOrientation;	// 0xC (12)
	int lfWeight;	// 0x10 (16)
	uint8 lfItalic;	// 0x14 (20)
	uint8 lfUnderline;	// 0x15 (21)
	uint8 lfStrikeOut;	// 0x16 (22)
	uint8 lfCharSet;	// 0x17 (23)
	uint8 lfOutPrecision;	// 0x18 (24)
	uint8 lfClipPrecision;	// 0x19 (25)
	uint8 lfQuality;	// 0x1A (26)
	uint8 lfPitchAndFamily;	// 0x1B (27)
	uint16 lfFaceNameStart;	// 0x1C (28)
	uint16 lfFaceNameEnd;	// 0x5A (90)
};

class MONITORINFO {
public:
	MONITORINFORef&& ref() const;
	uint self;
	uint cbSize;	// 0x0 (0)
	Rect rcMonitor;	// 0x4 (4)
	Rect rcWork;	// 0x14 (20)
	uint dwFlags;	// 0x24 (36)
};

class MSG {
public:
	MSGRef&& ref() const;
	uint self;
	uint hwnd;	// 0x0 (0)
	uint message;	// 0x4 (4)
	uint wParam;	// 0x8 (8)
	uint lParam;	// 0xC (12)
	uint time;	// 0x10 (16)
	Point pt;	// 0x14 (20)
};

class Rect {
public:
	Rect();
	RectRef&& ref() const;
	uint self;
	void ctor();
	int left;	// 0x0 (0)
	int top;	// 0x4 (4)
	int right;	// 0x8 (8)
	int bottom;	// 0xC (12)
};

class Point {
public:
	Point();
	PointRef&& ref() const;
	uint self;
	void ctor();
	int x;	// 0x0 (0)
	int y;	// 0x4 (4)
};

class Size {
public:
	~Size();
	Size();
	SizeRef&& ref() const;
	uint self;
	void ctor();
	void dtor();
	int cx;	// 0x0 (0)
	int cy;	// 0x4 (4)
};

class MINMAXINFO {
public:
	MINMAXINFORef&& ref() const;
	uint self;
	Point ptReserved;	// 0x0 (0)
	Point ptMaxSize;	// 0x8 (8)
	Point ptMaxPosition;	// 0x10 (16)
	Point ptMinTrackSize;	// 0x18 (24)
	Point ptMaxTrackSize;	// 0x20 (32)
};

class FILETIME {
public:
	FILETIMERef&& ref() const;
	uint self;
	uint dwLowDateTime;	// 0x0 (0)
	uint dwHighDateTime;	// 0x4 (4)
};

class WIN32_FIND_DATA {
public:
	WIN32_FIND_DATARef&& ref() const;
	uint self;
	uint dwFileAttributes;	// 0x0 (0)
	FILETIME ftCreationTime;	// 0x4 (4)
	FILETIME ftLastAccessTime;	// 0xC (12)
	FILETIME ftLastWriteTime;	// 0x14 (20)
	uint nFileSizeHigh;	// 0x1C (28)
	uint nFileSizeLow;	// 0x20 (32)
	uint dwReserved0;	// 0x24 (36)
	uint dwReserved1;	// 0x28 (40)
	int cFileName;	// 0x2C (44)
};

class GuidRef {
public:
	uint self;
	int operator<(uint);
	int operator<(GuidRef&&);
	GuidRef&& operator+(uint) const;
	Guid ref;	// 0x0 (0)
};

class TYPEATTRRef {
public:
	uint self;
	int operator<(uint);
	int operator<(TYPEATTRRef&&);
	TYPEATTRRef&& operator+(uint) const;
	TYPEATTR ref;	// 0x0 (0)
};

class FUNCDESCRef {
public:
	uint self;
	int operator<(uint);
	int operator<(FUNCDESCRef&&);
	FUNCDESCRef&& operator+(uint) const;
	FUNCDESC ref;	// 0x0 (0)
};

class SqliteTransactionGuardRef {
public:
	uint self;
	int operator<(uint);
	int operator<(SqliteTransactionGuardRef&&);
	SqliteTransactionGuardRef&& operator+(uint) const;
	SqliteTransactionGuard ref;	// 0x0 (0)
};

class LOGFONTRef {
public:
	uint self;
	int operator<(uint);
	int operator<(LOGFONTRef&&);
	LOGFONTRef&& operator+(uint) const;
	LOGFONT ref;	// 0x0 (0)
};

class MONITORINFORef {
public:
	uint self;
	int operator<(uint);
	int operator<(MONITORINFORef&&);
	MONITORINFORef&& operator+(uint) const;
	MONITORINFO ref;	// 0x0 (0)
};

class MSGRef {
public:
	uint self;
	int operator<(uint);
	int operator<(MSGRef&&);
	MSGRef&& operator+(uint) const;
	MSG ref;	// 0x0 (0)
};

class RectRef {
public:
	uint self;
	int operator<(uint);
	int operator<(RectRef&&);
	RectRef&& operator+(uint) const;
	Rect ref;	// 0x0 (0)
};

class PointRef {
public:
	uint self;
	int operator<(uint);
	int operator<(PointRef&&);
	PointRef&& operator+(uint) const;
	Point ref;	// 0x0 (0)
};

class SizeRef {
public:
	uint self;
	int operator<(uint);
	int operator<(SizeRef&&);
	SizeRef&& operator+(uint) const;
	Size ref;	// 0x0 (0)
};

class MINMAXINFORef {
public:
	uint self;
	int operator<(uint);
	int operator<(MINMAXINFORef&&);
	MINMAXINFORef&& operator+(uint) const;
	MINMAXINFO ref;	// 0x0 (0)
};

class FILETIMERef {
public:
	uint self;
	int operator<(uint);
	int operator<(FILETIMERef&&);
	FILETIMERef&& operator+(uint) const;
	FILETIME ref;	// 0x0 (0)
};

class WIN32_FIND_DATARef {
public:
	uint self;
	int operator<(uint);
	int operator<(WIN32_FIND_DATARef&&);
	WIN32_FIND_DATARef&& operator+(uint) const;
	WIN32_FIND_DATA ref;	// 0x0 (0)
};

class Variant {
public:
	~Variant();
	Variant();
	Variant(const Variant&);
	Variant(IDispatch&&);
	Variant& operator=(const Variant&);
	bool changeType(uint16 type);
	IDispatch&& getDispatch();
	void setDispatch(IDispatch&&);
	uint16 vt;	// 0x0 (0)
	uint8 byte;	// 0x8 (8)
	uint16 word;	// 0x8 (8)
	uint dword;	// 0x8 (8)
	uint64 qword;	// 0x8 (8)
	float real;	// 0x8 (8)
	double dreal;	// 0x8 (8)
};

class ActiveScript {
public:
	uint prepare(const string& engine, const string& source, uint startLine, const string& uName, bool bDebug, ScriptSite&& site);
	uint addNamedItem(const string& name, IUnknown&& object, bool isGlobal);
	IUnknown&& getObject();
	uint run();
	void stop();
	void abort();
	array<string>&& extractMacroses();
	bool invokeMacros(const string& name, Variant& res);
};

class ScriptSite {
public:
	bool onScriptError(const ScriptError& err);
	void enter();
	void leave();
};

class ASWnd {
public:
	void setMessages(const array<uint>& messages);
	uint doDefault();
};


// Globals funcs
void dumpVtable(?&, const string& alias = "");
bool checkInterface(?&);

namespace mem {
uint8 byte;
void set_byte(uint, uint8);
uint16 word;
void set_word(uint, uint16);
uint dword;
void set_dword(uint, uint);
uint64 qword;
void set_qword(uint, uint64);
uint size_t;
void set_size_t(uint, uint);
uint int_ptr;
void set_int_ptr(uint, uint);
float real;
void set_real(uint, float);
double dreal;
void set_dreal(uint, double);
uint addressOf(?&);
void memcpy(uint, uint, uint);
void memmove(uint, uint, uint);
void memset(uint, int, uint);
int interlockedIncr(uint address);
int interlockedDecr(uint address);
int interlockedExchangeAdd(uint address, int value);
int64 interlockedExchangeAdd64(uint address, int64 value);
uint malloc(uint size);
void free(uint ptr);

} // namespace mem
string join(const array<string>& strings, const string& delimeter, bool tail = false);
string formatInt(int64, const string& fmt = "", uint width = 0);
string formatFloat(double, const string& fmt, uint width, uint prec);
int64 parseInt(const string& val);
double parseFloat(const string& val);
string stringFromAddress(uint ptr, uint len = uint ( - 1 ));
string format(const string& pattern, const array<any>& args);
void Print(const string& message);
void Out(const string& message);
void initFuncDefFromAddress(uint address, ?& f, int callDecl = asCALL_CDECL);
void doLog(const string& message, uint logLevel = 2);
IUnknown&& toIUnknown(uint);
IDispatch&& toIDispatch(uint);
IConnectionPointContainer&& toIConnectionPointContainer(uint);
GuidRef&& toGuid(uint);
TYPEATTRRef&& toTYPEATTR(uint);
FUNCDESCRef&& toFUNCDESC(uint);
SqliteTransactionGuardRef&& toSqliteTransactionGuard(uint);
LOGFONTRef&& toLOGFONT(uint);
MONITORINFORef&& toMONITORINFO(uint);
MSGRef&& toMSG(uint);
RectRef&& toRect(uint);
PointRef&& toPoint(uint);
SizeRef&& toSize(uint);
MINMAXINFORef&& toMINMAXINFO(uint);
FILETIMERef&& toFILETIME(uint);
WIN32_FIND_DATARef&& toWIN32_FIND_DATA(uint);
uint SysAllocString(uint);
void SysFreeString(uint);
int sqlite3_open(uint name, uint& db);
int sqlite3_open_v2(uint name, uint& db, int flags, uint vfs = 0);
int sqlite3_close(uint db);
int sqlite3_close_v2(uint db);
int sqlite3_errcode(uint db);
int sqlite3_extended_errcode(uint db);
uint sqlite3_errmsg16(uint db);
int sqlite3_exec(uint db, uint utf8text, uint cb = 0, uint cbarg = 0, uint& errmsg);
int sqlite3_prepare16_v2(uint db, uint zSql, int nByte, uint& stmt, uint& pzTail);
uint sqlite3_db_handle(uint stmt);
int sqlite3_step(uint stmt);
int sqlite3_reset(uint stmt);
int sqlite3_finalize(uint stmt);
int sqlite3_changes(uint db);
int sqlite3_total_changes(uint db);
int64 sqlite3_last_insert_rowid(uint db);
int sqlite3_column_count(uint stmt);
uint sqlite3_column_name16(uint stmt, int nColumn);
uint sqlite3_column_blob(uint stmt, int iCol);
int sqlite3_column_bytes(uint stmt, int iCol);
int sqlite3_column_bytes16(uint stmt, int iCol);
double sqlite3_column_double(uint stmt, int iCol);
int sqlite3_column_int(uint stmt, int iCol);
int64 sqlite3_column_int64(uint stmt, int iCol);
uint sqlite3_column_text(uint stmt, int iCol);
uint sqlite3_column_text16(uint stmt, int iCol);
int sqlite3_column_type(uint stmt, int iCol);
int sqlite3_clear_bindings(uint stmt);
int sqlite3_bind_parameter_count(uint stmt);
int sqlite3_bind_parameter_index(uint stmt, uint utf8Name);
uint sqlite3_bind_parameter_name(uint stmt, int);
int sqlite3_bind_blob(uint stmt, int, uint data, int n, int destr = SQLITE_TRANSIENT);
int sqlite3_bind_double(uint stmt, int, double);
int sqlite3_bind_int(uint stmt, int, int);
int sqlite3_bind_int64(uint stmt, int, int64);
int sqlite3_bind_null(uint stmt, int);
int sqlite3_bind_text(uint stmt, int, uint text, int nLen, int destr = SQLITE_TRANSIEN);
int sqlite3_bind_text16(uint stmt, int, uint text, int nLen, int destr = SQLITE_TRANSIEN);
int SetWindowText(uint, uint);
int GetWindowTextLength(uint hWnd);
int GetWindowText(uint hWnd, uint lpString, int nMaxCount);
void keybd_event(uint8 bVk, uint8 bScan, uint dwFlags, uint dwExtraInfo);
uint MapVirtualKey(uint uCode, uint uMapType);
uint SetFocus(uint hWnd);
uint GetFocus();
uint SendMessage(uint, uint, uint = 0, uint = 0);
int PostMessage(uint, uint, uint = 0, uint = 0);
int DestroyWindow(uint hWnd);
int ShowWindow(uint hWnd, int nCmdShow);
int UpdateWindow(uint hWnd);
uint16 GetKeyState(int nVirtKey);
int SetWindowPos(uint hWnd, uint hWndInsertAfter, int X, int Y, int cx, int cy, uint uFlags);
uint GetDC(uint hWnd);
int GetCaretPos(Point& lpPoint);
uint GetDesktopWindow();
int ReleaseDC(uint hWnd, uint hDC);
int GetSystemMetrics(int nIndex);
int ClientToScreen(uint hWnd, Point& lpPoint);
int ScreenToClient(uint hWnd, Point& lpPoint);
int CreateCaret(uint hWnd, uint hBitmap, int nWidth, int nHeight);
int SetCaretPos(int X, int Y);
int ShowCaret(uint hWnd);
int DestroyCaret();
int GetClassName(uint hWnd, uint lpClassName, int nMaxCount);
uint FindWindow(uint lpClassName, uint lpWindowName);
int IsWindowVisible(uint hWnd);
int PeekMessage(MSG& lpMsg, uint hWnd, uint wMsgFilterMin, uint wMsgFilterMax, uint wRemoveMsg);
uint DialogBoxIndirectParam(uint hInstance, uint hDialogTemplate, uint hWndParent, uint lpDialogFunc, uint dwInitParam);
int EndDialog(uint hDlg, uint nResult);
int GetWindowRect(uint hWnd, Rect& lpRect);
uint CreateWindowEx(uint dwExStyle, uint lpClassName, uint lpWindowName, uint dwStyle, int X, int Y, int nWidth, int nHeight, uint hWndParent, uint hMenu, uint hInstance, uint lpParam);
int MoveWindow(uint hWnd, int X, int Y, int nWidth, int nHeight, int bRepaint);
int GetClientRect(uint hWnd, Rect& lpRect);
int AdjustWindowRectEx(Rect& lpRect, uint dwStyle, int bMenu, uint dwExStyle);
int GetWindowLong(uint hWnd, int nIndex);
int DefWindowProc(uint hWnd, uint msg, int wParam, int lParam);
int TranslateMessage(MSG& lpMsg);
uint CreateFontIndirect(LOGFONT& lplf);
uint SelectObject(uint hdc, uint h);
int GetTextExtentPoint32(uint hdc, uint lpString, int c, Size&);
int DeleteObject(uint ho);
int GetDeviceCaps(uint hdc, int index);
uint GetStockObject(int i);
int GetObjectW(uint h, uint c, uint pv);
int EnumFontFamiliesEx(uint hdc, LOGFONT& lpLogfont, uint lpProc, NoCaseMap<int>&, uint dwFlags);
void Sleep(uint);
uint GetTickCount();
int VirtualProtect(uint address, uint size, uint newProtect, uint& oldProtect);
int MultiByteToWideChar(uint CodePage, uint dwFlags, uint lpMultiByteStr, int cbMultiByte, uint lpWideCharStr, int cchWideChar);
int WideCharToMultiByte(uint CodePage, uint dwFlags, uint lpWideCharStr, int cchWideChar, uint lpMultiByteStr, int cbMultiByte, uint lpDefaultChar, bool& lpUsedDefaultChar);
uint GetCommandLine();
uint ExpandEnvironmentStrings(uint lpSrc, uint buffer, uint nSize);
uint GetTempPath(uint nBufferLength, uint lpBuffer);
int CreateDirectory(uint lpPathName, uint = 0);
uint LoadLibraryEx(uint lpLibFileName, uint = 0, uint dwFlags = 0);
uint GetProcAddress(uint hModule, uint lpProcName);
void DebugBreak();
uint GetModuleHandle(uint lpModuleName);
uint GetLastError();
uint GetFileAttributes(uint path);
uint FindFirstFile(uint path, uint lpFindFileData);
int FindNextFile(uint hFindFile, uint lpFindFileData);
int FindClose(uint hFindFile);
uint CreateFile(uint lpFileName, uint dwDesiredAccess, uint dwShareMode, uint lpSecurityAttributes, uint dwCreationDisposition, uint dwFlagsAndAttributes, uint hTemplateFile);
uint GetFileSize(uint hFile, uint lpFileSizeHigh = 0);
int ReadFile(uint hFile, uint lpBuffer, uint nNumberOfBytesToRead, uint lpNumberOfBytesRead = 0, uint lpOverlapped = 0);
int WriteFile(uint hFile, uint lpBuffer, uint nNumberOfBytesToWrite, uint lpNumberOfBytesWritten = 0, uint lpOverlapped = 0);
int CloseHandle(uint hObject);
void ExitProcess(uint uExitCode);
int TerminateProcess(uint hProcess, uint uExitCode);
uint GetCurrentProcess();
IDispatch&& createDispatchFromAS(?&);
IDispatch&& getEnumsDispatch();
void setComException(const string& description);
IUnknown&& AStoIUnknown(?&, const Guid&);
uint ThunkToFunc(?&, int callType = asCALL_STDCALL);
void initActiveScriptSubsystem(uint hMainWindow, const string& nameOfInstance);
void unloadDelayedScripts();
string findFullPath(const string& path);
string getEngineNameByFileExt(const string& ext);
ASWnd&& attachWndToFunction(uint hWnd, WndFunc&& handler, const array<uint>& messages);

// Globals props
as_env env;
Guid IID_IUnknown;
Guid IID_IDispatch;
Guid IID_IConnectionPointContainer;
Guid IID_ITypeInfo;
