#include "UUIDGenerator.h"

#include <rpc.h>
#include <windows.h>

#include <stdexcept>
#include <string>

std::string generateUUID() {
  UUID ID_generated;

  RPC_STATUS status = UuidCreate(&ID_generated);

  if (status != RPC_S_OK && status != RPC_S_UUID_LOCAL_ONLY) {
    throw std::runtime_error("UUID generation failed");
  }

  RPC_CSTR uuidstring = nullptr;

  RPC_STATUS result = UuidToStringA(&ID_generated, &uuidstring);

  if (result != RPC_S_OK) {
    throw std::runtime_error("UUID conversion failed");
  }

  std::string finalUUID = reinterpret_cast<const char*>(uuidstring);

  RpcStringFreeA(&uuidstring);
  return finalUUID;
}
