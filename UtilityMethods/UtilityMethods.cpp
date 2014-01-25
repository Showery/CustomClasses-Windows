#include "UtilityMethods.h"
#include "showery-safe-free.h"

//////////////////////////////////////////////////////////////////////////
///For both PlayerCore and MediaPlayer.

//����ϵͳ�汾�Ƿ�ΪXP�����
bool isXPorEarlier()
{
    OSVERSIONINFO os_version;
    bool is_XP_or_later = FALSE;
    memset(&os_version, 0, sizeof(OSVERSIONINFO));
    os_version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&os_version);
    return (os_version.dwMajorVersion<6) ? true : false;
}

//������׷�ӵ����ļ�ĩβ��
void SaveDataToFile(const char *file_path, void *data, DWORD size)
{
    FILE *file_handle = nullptr;
    fopen_s(&file_handle, file_path, "ab+");
    if(nullptr!=file_handle) {
        if(0<size) {
            fseek(file_handle, 0, SEEK_END);
            fwrite(data, 1, size, file_handle);
        }
    }
    fclose(file_handle);
}

///================================================================
///Only for MediaPlayer.

//////////////////////////////////////////////////////////////////////////
///�ַ�������غ���
//ȥ��mrl��file:\\\ǰ׺����û�д�ǰ׺��ֱ�Ӹ���
//��������Ҫ�ͷŷ���ֵ�ڴ�
char* mrlToFilePath(const char *mrl)
{
    if(nullptr == mrl) return nullptr;
    char *ret = nullptr;
    if(mrl ==strstr(mrl, "file:///")) {
        char *temp_mrl = _strdup(mrl);
        int path_len = strlen(mrl) - strlen("file:///");
        //TODO:
    }
    else if(mrl == strstr(mrl, "file:\\\\\\")){
        char *temp_mrl = _strdup(mrl);
        int path_len = strlen(mrl) - strlen("file:\\\\\\");
        char *path = temp_mrl + strlen("file:\\\\\\");
        ret = _strdup(path);
        SafeDeleteArray(&temp_mrl);
    }
    else {
        ret = _strdup(mrl);
    } 
    return ret;
}

//��ͼ�����ļ�·�������ļ����Դ��ж��ļ�·���Ƿ����
bool isFileExists(const char *path)
{
    WIN32_FIND_DATA find_file_data;
    HANDLE find_file = FindFirstFile(path, &find_file_data);
    if(INVALID_HANDLE_VALUE != find_file) {
        FindClose(find_file);
        return true;
    } else {
        return false;
    }
}

//���ֽ��ַ���ת���ַ�
//�ú���Ϊ���ַ������ַ������ڴ�
//ʹ������Ҫ�ͷŷ���ֵ�ڴ�
wchar_t* charToWCHAR(const char *str)
{
    wchar_t *ret = nullptr;
    DWORD size = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
    ret = new wchar_t[size];
    MultiByteToWideChar(CP_ACP, 0, str, -1, ret, size);
    return ret;
}

//���ַ����а�������·����ɵ����룬���޸�֮
std::string uriEncode(const std::string& s)
{
    char hex[] = "0123456789ABCDEF";
    std::string dst;

    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char cc = s[i];
        if (isascii(cc)) {
            if (cc == ' ') {
                dst += "%20";
            } else {
                dst += cc;
            }
        } else {
            unsigned char c = static_cast<unsigned char>(s[i]);
            dst += '%';
            dst += hex[c / 16];
            dst += hex[c % 16];
        }
    }
    return dst;
}

//�����Windowsƽ̨�����ַ���ת��ΪUTF-8����
//����ֱ�ӷ���
#if defined(WIN32) || defined(WIN64)
int ansiToUtf8(const char* ansi, char* utf8, size_t size)
{
    wchar_t tmp[1024]= {0};     
    int wl = MultiByteToWideChar(CP_ACP, 0, ansi, strlen(ansi), tmp, 1024); 
    int ml = WideCharToMultiByte(CP_UTF8, 0, tmp, wl, utf8, size, nullptr, nullptr);
    utf8[ml] = '\0';
    return ml;
}
#else
int ansiToUtf8(const char* ansi, char* utf8, size_t size)
{
    return 0;
}
#endif

//��߱ȵĸ�����ֵת��ΪX:Y���ַ�����ʽ
//�����߲���ҪҲ�������ͷŷ���ֵ�ڴ�
const char *floatToAspectRatio(float val)
{
    if(val < 0.000001f)
        return "";
    if (val >0.0f && val < 1.24f)
        return "16:13";
    if (val > 1.24f && val < 1.30f)
        return "5:4";
    if (val > 1.30f && val < 1.40f)
        return "4:3";
    if (val > 1.45f && val < 1.55f)
        return "3:2";
    if (val > 1.55f && val < 1.65f)
        return "16:10";
    if (val > 1.65f && val < 1.95f)
        return "16:9";
    if (val > 1.95f && val < 2.05f)
        return "2:1";
    if (val > 2.05f && val < 2.25f)
        return "221:100";
    if (val > 2.25f && val < 2.30f)
        return "16:7";
    if (val > 2.30f && val < 2.37f)
        return "235:100";
    if (val > 2.37f && val < 2.40f)
        return "239:100";
    if (val > 2.45f && val < 2.55f)
        return "5:2";
    if (val > 2.90f && val < 3.10f)
        return "3:1";
    return "";
}

//////////////////////////////////////////////////////////////////////////
///DirectShow ����Graph��غ���

//����base_filter����Pin���ҷ���ָ�������Pin
//base_filter����ֻ��һ������ָ�������Pinʱ���øú���
HRESULT findOnlyPin(IBaseFilter *base_filter, IPin **ret_pin, const PIN_DIRECTION find_dir)
{
    if(nullptr==base_filter) {
        return E_POINTER;
    }

    HRESULT hr = S_OK;
    CComPtr<IEnumPins> enum_pins;
    hr = base_filter->EnumPins(&enum_pins);
    if(FAILED(hr)) {
        return hr;
    }
    CComPtr<IPin> temp_pin;
    enum_pins->Reset();
    while(SUCCEEDED(enum_pins->Next(1, &temp_pin, nullptr))) {
        //��ֹ���������
        if(nullptr==temp_pin) break;
        PIN_DIRECTION pin_dir;
        temp_pin->QueryDirection(&pin_dir);
        if(find_dir != pin_dir) {
            temp_pin = nullptr;
            continue;
        }
        temp_pin.CopyTo(ret_pin);
        //This method needs to be called.
        //Because we are going to release temp_pin.
        //And the "=" operation doesn't increase reference number.
        temp_pin = nullptr;
        break;
    }

    if(nullptr==(*ret_pin))
        return E_FAIL;
    else
        return S_OK;
}

//����findOnlyPin�������Pin
HRESULT findInputPin(IBaseFilter *base_filter, IPin **input_pin)
{
    return findOnlyPin(base_filter, input_pin, PINDIR_INPUT);
}

//����findOnlyPin�������Pin
HRESULT findOutPin(IBaseFilter *base_filter, IPin **out_pin)
{
    return findOnlyPin(base_filter, out_pin, PINDIR_OUTPUT);
}

//����base_filter����Pin�������з���ָ�����ֺͷ����Pin
HRESULT findPinByName(IBaseFilter *base_filter, IPin **ret_pin, const PIN_DIRECTION find_dir, LPCOLESTR pin_name)
{
    HRESULT hr = S_OK;
    CComPtr<IEnumPins> enum_pins;
    hr = base_filter->EnumPins(&enum_pins);
    if(FAILED(hr)) {
        return hr;
    }
    CComPtr<IPin> temp_pin;
    enum_pins->Reset();
    while(SUCCEEDED(enum_pins->Next(1, &temp_pin, nullptr))) {
        //��ֹ���������
        if(nullptr==temp_pin) break;
        PIN_DIRECTION pin_dir;
        temp_pin->QueryDirection(&pin_dir);
        if(find_dir != pin_dir) {
            temp_pin = nullptr;
            continue;
        }
        PIN_INFO pin_info;
        temp_pin->QueryPinInfo(&pin_info);
        if(0==_wcsicmp(pin_name, pin_info.achName)) {
            SafeRelease(&pin_info.pFilter);
            temp_pin.CopyTo(ret_pin);
            break;
        }
        SafeRelease(&pin_info.pFilter);
        temp_pin = nullptr;
    }

    if(nullptr==(*ret_pin))
        return E_FAIL;
    else
        return S_OK;
}

//����findPinByName��������Pin
HRESULT findInputPinByName(IBaseFilter *base_filter, IPin **input_pin, LPCOLESTR pin_name)
{
    return findPinByName(base_filter, input_pin, PINDIR_INPUT, pin_name);
}

//����findPinByName�������Pin
HRESULT findOutPinByName(IBaseFilter *base_filter, IPin **out_pin, LPCOLESTR pin_name)
{
    return findPinByName(base_filter, out_pin, PINDIR_OUTPUT, pin_name);
}

//�ҳ�base_filter�ĵ�һ������pin����out_pin���ӵ�������pin
HRESULT connectTwoFilters(IGraphBuilder *graph_builder, IPin *out_pin, IBaseFilter *base_filter)
{
    if(nullptr==graph_builder || nullptr==out_pin || nullptr==base_filter) return E_POINTER;

    HRESULT hr = S_OK;
    CComPtr<IPin> input_pin;
    hr = findInputPin(base_filter, &input_pin);
    if(FAILED(hr) || nullptr==input_pin) {
        return E_FAIL;
    }
    hr = graph_builder->Connect(out_pin, input_pin);
    return hr;
}

//�ҳ�first_filter��Ψһ���pin��second_filter�ĵ�һ������pin����������pin
HRESULT connectTwoFilters(IGraphBuilder *graph_builder, IBaseFilter *first_filter, IBaseFilter *second_filter)
{
    if(nullptr==graph_builder || nullptr==first_filter || nullptr==second_filter) {
        return E_POINTER;
    }

    HRESULT hr = S_OK;
    CComPtr<IPin> input_pin, out_pin;
    hr = findOutPin(first_filter, &out_pin);
    if(FAILED(hr) || nullptr==out_pin) {
        return E_FAIL;
    }
    hr = findInputPin(second_filter, &input_pin);
    if(FAILED(hr) || nullptr==input_pin) {
        return E_FAIL;
    }
    hr = graph_builder->Connect(out_pin, input_pin);

    return hr;
}

//���������ӵ����롢���Pin���Ͽ����ǵ�����
void disconnectFilter(IBaseFilter *base_filter, IGraphBuilder *graph_builder)
{
    if(nullptr==base_filter || nullptr==graph_builder) return;

    HRESULT hr = S_OK;
    CComPtr<IEnumPins> enum_pins;
    hr = base_filter->EnumPins(&enum_pins);
    if(FAILED(hr)) return;
    enum_pins->Reset();
    CComPtr<IPin> cur_pin, connect_pin;
    while(SUCCEEDED(enum_pins->Next(1, &cur_pin, nullptr))) {
        // ��ֹ���������
        if(nullptr == cur_pin) break;
        cur_pin->ConnectedTo(&connect_pin);
        if(connect_pin) {
            graph_builder->Disconnect(connect_pin);
            graph_builder->Disconnect(cur_pin);
        }
        cur_pin = nullptr;
        connect_pin = nullptr;
    }
    //��graph builder�������filter
    graph_builder->RemoveFilter(base_filter);

    return;
}

//����base_filter��ָ�����Ƶ�pin������pin�����ӣ��Ͽ�������
void disconnectPinByName(IBaseFilter *base_filter, LPCOLESTR pin_name)
{
    if(nullptr == base_filter) return;

    HRESULT hr = S_OK;
    CComPtr<IEnumPins> enum_pins;
    hr = base_filter->EnumPins(&enum_pins);
    if(FAILED(hr)) return;
    enum_pins->Reset();
    CComPtr<IPin> cur_pin, connect_pin;
    while(SUCCEEDED(enum_pins->Next(1, &cur_pin, nullptr))) {
        // ��ֹ���������
        if(nullptr == cur_pin) break;
        // �жϸ�pin�����Ƿ�Ϊָ������
        PIN_INFO pin_info;
        memset(&pin_info, 0, sizeof(pin_info));
        cur_pin->QueryPinInfo(&pin_info);
        SafeRelease(&pin_info.pFilter);
        if(0!=_wcsicmp(pin_name, pin_info.achName)) {
            cur_pin = nullptr;
            connect_pin = nullptr;
            continue;
        }
        // �жϸ�pin�Ƿ�������
        cur_pin->ConnectedTo(&connect_pin);
        if(connect_pin) {
            connect_pin->Disconnect();
            cur_pin->Disconnect();
        }
        cur_pin = nullptr;
        connect_pin = nullptr;
    }
}

///================================================================
///Only for PlayerCore.