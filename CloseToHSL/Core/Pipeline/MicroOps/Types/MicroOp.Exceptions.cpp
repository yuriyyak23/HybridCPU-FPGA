#include "MicroOp.Exceptions.h"





#include <codecvt>
#include <locale>
namespace YAKSys_Hybrid_CPU::Core {
namespace { std::string ToUtf8(const std::wstring &s) { return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(s); } }
unsigned long long PageFaultException::getFaultAddress() const {return FaultAddress;}
bool PageFaultException::isWrite() const {return IsWrite;}
std::exception_ptr PageFaultException::getInnerException() const noexcept {return InnerException;}
PageFaultException::PageFaultException(unsigned long long address, bool isWrite)
    : std::runtime_error(std::format("Page fault at address 0x{:X} during {}", address, isWrite ? "write" : "read")), FaultAddress(address), IsWrite(isWrite), InnerException(nullptr) {}
PageFaultException::PageFaultException(const std::wstring &message, unsigned long long address, bool isWrite)
    : std::runtime_error(ToUtf8(message)), FaultAddress(address), IsWrite(isWrite), InnerException(nullptr) {}
PageFaultException::PageFaultException(const std::wstring &message, std::runtime_error innerException, unsigned long long address, bool isWrite)
    : std::runtime_error(ToUtf8(message)), FaultAddress(address), IsWrite(isWrite), InnerException(std::make_exception_ptr(innerException)) {}
int DomainFaultException::getVirtualThreadId() const {return VirtualThreadId;}
unsigned long long DomainFaultException::getFaultingPC() const {return FaultingPC;}
unsigned long long DomainFaultException::getOperationDomainTag() const {return OperationDomainTag;}
unsigned long long DomainFaultException::getActiveCert() const {return ActiveCert;}
DomainFaultException::DomainFaultException(int vtId, unsigned long long pc, unsigned long long opTag, unsigned long long cert)
    : std::runtime_error(std::format("FAULT_DOMAIN_VT[{}]: domain tag 0x{:X} rejected by cert 0x{:X} at PC=0x{:X}",vtId,opTag,cert,pc)), VirtualThreadId(vtId), FaultingPC(pc), OperationDomainTag(opTag), ActiveCert(cert) {}
}