//������Ҫ�õ��ĺ���
#pragma once
// For CComPtr
#include <atlbase.h>
#include <DShow.h>
#include <string>

//����ϵͳ�Ƿ�ΪXP�����汾
bool isXPorEarlier();

//�����ݱ��浽�ļ���
void SaveDataToFile(const char *file_path, void *data, DWORD size);

//mrlת�ļ�·��
char* mrlToFilePath(const char *mrl);

//�Ƿ��ļ�·��
bool isFileExists(const char *path);

//���ֽ�ת���ַ�
wchar_t* charToWCHAR(const char *str);

//��������·������
std::string uriEncode(const std::string& s);

//ANSIתUTF-8
int ansiToUtf8(const char* ansi, char* utf8, size_t size);

//�����Ϳ�߱�ת��Ϊ�ַ�����ʽ
const char* floatToAspectRatio(float val);

//����Ψһ��Pin
HRESULT findOnlyPin(IBaseFilter *base_filter, IPin **ret_pin, const PIN_DIRECTION find_dir);

//����Ψһ������Pin
HRESULT findInputPin(IBaseFilter *base_filter, IPin **input_pin);

//����Ψһ�����Pin
HRESULT findOutPin(IBaseFilter *base_filter, IPin **out_pin);

//�������ֺͷ������Pin
HRESULT findPinByName(IBaseFilter *base_filter, IPin **ret_pin, const PIN_DIRECTION find_dir, LPCOLESTR pin_name);

//�������ֲ�������Pin
HRESULT findInputPinByName(IBaseFilter *base_filter, IPin **input_pin, LPCOLESTR pin_name);

//�������ֲ������Pin
HRESULT findOutPinByName(IBaseFilter *base_filter, IPin **out_pin, LPCOLESTR pin_name);

//�����pin���ӵ�base_filter������pin
HRESULT connectTwoFilters(IGraphBuilder *graph_builder, IPin *out_pin, IBaseFilter *base_filter);

//��������filters���ӵ�һ������ڶ���
HRESULT connectTwoFilters(IGraphBuilder *graph_builder, IBaseFilter *first_filter, IBaseFilter *second_filter);

//�Ͽ�base_filter�����ӣ��������graph builder���Ƴ�
void disconnectFilter(IBaseFilter *base_filter, IGraphBuilder *graph_builder);

//�Ͽ�base_filterָ�����Ƶ�pin
void disconnectPinByName(IBaseFilter *base_filter, LPCOLESTR pin_name);