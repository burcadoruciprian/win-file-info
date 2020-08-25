#pragma comment(lib, "Version")

#include <node.h>
#include <Windows.h>
#include <string>
#include <memory>
#include <node_version.h>

#define NODE_7_0_MODULE_VERSION  51

using v8::Array;
using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;
using v8::NewStringType;

void GetFileVersion(const FunctionCallbackInfo<Value> &args)
{
  Isolate *isolate = args.GetIsolate();

  if (args.Length() != 1)
  {
    // Throw an Error that is passed back to JavaScript
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments", NewStringType::kNormal).ToLocalChecked()));
    return;
  }

  if (!args[0]->IsString())
  {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong argument type. File path expected", NewStringType::kNormal).ToLocalChecked()));
    return;
  }

#if NODE_MODULE_VERSION > NODE_7_0_MODULE_VERSION
  String::Utf8Value arg0(isolate, args[0]);
#else
  String::Utf8Value arg0(args[0]);
#endif
  std::string temp = std::string(*arg0);
  std::wstring filePath = std::wstring(temp.begin(), temp.end());

  DWORD versionHandle = NULL;
  DWORD versionDataSize = ::GetFileVersionInfoSizeW(filePath.c_str(), &versionHandle);
  if (NULL == versionDataSize)
  {
    std::string errMsg = "::GetFileVersionInfoSize failed. Error: " + std::to_string(GetLastError());
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, errMsg.c_str(), NewStringType::kNormal).ToLocalChecked()));
    return;
  }

  std::unique_ptr<BYTE[]> versionData(new BYTE[versionDataSize]);
  BOOL ret = ::GetFileVersionInfoW(filePath.c_str(), versionHandle, versionDataSize, versionData.get());
  if (!ret)
  {
    std::string errMsg = "::GetFileVersionInfo failed. Error: " + std::to_string(GetLastError());
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, errMsg.c_str(), NewStringType::kNormal).ToLocalChecked()));
    return;
  }

  UINT size = 0;
  VS_FIXEDFILEINFO *pVerInfo = nullptr;
  ret = ::VerQueryValueW(versionData.get(), L"\\", (LPVOID *)&pVerInfo, &size);
  if (!ret)
  {
    std::string errMsg = "::VerQueryValue failed. Error: " + std::to_string(GetLastError());
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, errMsg.c_str(), NewStringType::kNormal).ToLocalChecked()));
    return;
  }

  int major = HIWORD(pVerInfo->dwFileVersionMS);
  int minor = LOWORD(pVerInfo->dwFileVersionMS);
  int buildMajor = HIWORD(pVerInfo->dwFileVersionLS);
  int buildMinor = LOWORD(pVerInfo->dwFileVersionLS);

  // we'll populate this with the results
  std::string version = std::to_string(major);
  version.append(".");
  version.append(std::to_string(minor));
  version.append(".");
  version.append(std::to_string(buildMajor));
  version.append(".");
  version.append(std::to_string(buildMinor));

  args.GetReturnValue().Set(String::NewFromUtf8(isolate,version.c_str(), NewStringType::kNormal).ToLocalChecked());
}

void Init(Local<Object> exports)
{
  NODE_SET_METHOD(exports, "getFileVersion", GetFileVersion);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init);