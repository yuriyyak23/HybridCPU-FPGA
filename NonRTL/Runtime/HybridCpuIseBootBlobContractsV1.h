#pragma once
#include "../../CloseToHSL/Core/ManagedRuntime/HybridCpuManagedGcRuntimeV1.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
namespace HybridCPU::RuntimeKernel { class IHybridCpuRuntimeKernelV1; }
namespace HybridCPU::ManagedRuntime { class HybridCpuManagedStringRuntimeV1; class HybridCpuManagedNonMovingGcV1; }
namespace YAKSys_Hybrid_CPU::Core { class IHybridCpuIseEcallBridgeV1; }
namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed {
class HybridCpuManagedBootstrapResultV1; class HybridCpuManagedStackMapRegistrationV1; struct HybridCpuIseManagedInitializerBindingV1;
}
namespace HybridCPU::Platform::Contracts {
enum class HybridCpuKernelStatusV1 : unsigned char { Success=0, InvalidRequest=1, VersionMismatch=2, AddressConflict=3, BudgetExhausted=4, Unsupported=5, NoCurrentContext=6, TrapStateMismatch=7, ProcessExited=8 };
enum class HybridCpuManagedTypeKindV1 : unsigned char { Class=0, Interface=1, SzArray=2, String=3, ValueType=4 };
enum class HybridCpuManagedStorageKindV1 : unsigned char { Primitive=0, ObjectReference=1, BlittableValue=2 };
enum class HybridCpuManagedTypeInitializationStateV1 : unsigned char { Uninitialized=0, Running=1, Initialized=2, Failed=3 };
struct HybridCpuModuleInitializerRegistrationV1 final {
 std::wstring ModuleIdentity, InitializerSymbol;
 int Order;
 std::optional<unsigned long long> TypeId;
 HybridCpuModuleInitializerRegistrationV1(std::wstring moduleIdentity,std::wstring initializerSymbol,int order,
  std::optional<unsigned long long> typeId=std::nullopt)
  :ModuleIdentity(std::move(moduleIdentity)),InitializerSymbol(std::move(initializerSymbol)),Order(order),TypeId(typeId){}
};
enum class HybridCpuHostServiceV1 : unsigned short { ProcessExit=1, Console=2, File=3, Network=4, Clock=5, Graphics=6, Input=7, ManagedRuntime=8 };
enum class HybridCpuHostBufferAccessV1 : unsigned char { None=0, Read=1, Write=2, ReadWrite=3 };
enum class HybridCpuExternalServiceStatusV1 : unsigned char { Success=0, InvalidRequest=1, AccessDenied=2, MissingProvider=3, MissingSymbol=4, ProviderFailure=5, ManagedExceptionContained=6, ReentrancyRejected=7, Disabled=8 };
enum class HybridCpuPrivilegeModeV1 : unsigned char { User=0, Supervisor=1, Machine=2 };
class HybridCpuManagedFieldLayoutV1 final { public: std::wstring Identity; HybridCpuManagedStorageKindV1 StorageKind; int OffsetBytes,SizeBytes,AlignmentBytes; HybridCpuManagedFieldLayoutV1(std::wstring i,HybridCpuManagedStorageKindV1 s,int o,int z,int a):Identity(std::move(i)),StorageKind(s),OffsetBytes(o),SizeBytes(z),AlignmentBytes(a){} };
class HybridCpuManagedStaticLayoutV1 final { public: int SizeBytes,AlignmentBytes; std::shared_ptr<std::vector<std::shared_ptr<HybridCpuManagedFieldLayoutV1>>> Fields; std::shared_ptr<std::vector<int>> ObjectReferenceOffsets; HybridCpuManagedStaticLayoutV1(int s,int a,std::shared_ptr<std::vector<std::shared_ptr<HybridCpuManagedFieldLayoutV1>>> f,std::shared_ptr<std::vector<int>> o):SizeBytes(s),AlignmentBytes(a),Fields(std::move(f)),ObjectReferenceOffsets(std::move(o)){} };
class HybridCpuManagedArrayShapeV1 final { public: HybridCpuManagedStorageKindV1 ElementStorageKind; std::optional<unsigned long long> ElementTypeId; int ElementSizeBytes,ElementAlignmentBytes,LengthOffsetBytes,DataOffsetBytes; bool IsSzArray,RequiresReferenceStoreCheck; HybridCpuManagedArrayShapeV1(HybridCpuManagedStorageKindV1 s,std::optional<unsigned long long> t,int e,int a,int l,int d,bool z,bool r):ElementStorageKind(s),ElementTypeId(t),ElementSizeBytes(e),ElementAlignmentBytes(a),LengthOffsetBytes(l),DataOffsetBytes(d),IsSzArray(z),RequiresReferenceStoreCheck(r){} };
class HybridCpuManagedStringShapeV1 final { public: int LengthOffsetBytes,DataOffsetBytes,CharacterSizeBytes; bool IsImmutable; HybridCpuManagedStringShapeV1(int l,int d,int c,bool i):LengthOffsetBytes(l),DataOffsetBytes(d),CharacterSizeBytes(c),IsImmutable(i){} };
class HybridCpuManagedValueTypeShapeV1 final { public: int PayloadSizeBytes,PayloadAlignmentBytes; std::shared_ptr<std::vector<int>> ObjectReferenceOffsets; int BoxedPayloadOffsetBytes; HybridCpuManagedValueTypeShapeV1(int s,int a,std::shared_ptr<std::vector<int>> o,int b):PayloadSizeBytes(s),PayloadAlignmentBytes(a),ObjectReferenceOffsets(std::move(o)),BoxedPayloadOffsetBytes(b){} };
class HybridCpuManagedTypeDescriptorV1 final {
public:
 std::wstring SchemaId; int SchemaMajor,SchemaMinor; unsigned long long TypeId; std::wstring StableIdentity; HybridCpuManagedTypeKindV1 Kind; std::optional<unsigned long long> BaseTypeId;
 std::shared_ptr<std::vector<unsigned long long>> InterfaceTypeIds; int InstanceSizeBytes,InstanceAlignmentBytes;
 std::shared_ptr<std::vector<std::shared_ptr<HybridCpuManagedFieldLayoutV1>>> InstanceFields; std::shared_ptr<HybridCpuManagedStaticLayoutV1> StaticLayout;
 std::shared_ptr<std::vector<unsigned long long>> VirtualSlotMethodIds,InterfaceSlotMethodIds; std::wstring DescriptorDigest;
 std::shared_ptr<HybridCpuManagedArrayShapeV1> ArrayShape; std::shared_ptr<HybridCpuManagedStringShapeV1> StringShape; std::shared_ptr<HybridCpuManagedValueTypeShapeV1> ValueTypeShape;
 HybridCpuManagedTypeDescriptorV1(std::wstring sid,int major,int minor,unsigned long long id,std::wstring identity,HybridCpuManagedTypeKindV1 kind,std::optional<unsigned long long> base,
 std::shared_ptr<std::vector<unsigned long long>> interfaces,int size,int align,std::shared_ptr<std::vector<std::shared_ptr<HybridCpuManagedFieldLayoutV1>>> fields,std::shared_ptr<HybridCpuManagedStaticLayoutV1> statics,
 std::shared_ptr<std::vector<unsigned long long>> virtuals,std::shared_ptr<std::vector<unsigned long long>> interfaceSlots,std::wstring digest,std::shared_ptr<HybridCpuManagedArrayShapeV1> array,
 std::shared_ptr<HybridCpuManagedStringShapeV1> string,std::shared_ptr<HybridCpuManagedValueTypeShapeV1> value)
 :SchemaId(std::move(sid)),SchemaMajor(major),SchemaMinor(minor),TypeId(id),StableIdentity(std::move(identity)),Kind(kind),BaseTypeId(base),InterfaceTypeIds(std::move(interfaces)),InstanceSizeBytes(size),InstanceAlignmentBytes(align),InstanceFields(std::move(fields)),StaticLayout(std::move(statics)),VirtualSlotMethodIds(std::move(virtuals)),InterfaceSlotMethodIds(std::move(interfaceSlots)),DescriptorDigest(std::move(digest)),ArrayShape(std::move(array)),StringShape(std::move(string)),ValueTypeShape(std::move(value)){}
};
class HybridCpuExternalServiceRequestV1 final { public: unsigned long long ContextId; HybridCpuHostServiceV1 Service; unsigned long long Operation; HybridCpuPrivilegeModeV1 PrivilegeMode; unsigned long long BufferAddress,BufferLength; HybridCpuHostBufferAccessV1 BufferAccess; std::shared_ptr<std::vector<unsigned long long>> Arguments; std::wstring TransitionDigest; HybridCpuExternalServiceRequestV1(unsigned long long c,HybridCpuHostServiceV1 s,unsigned long long o,HybridCpuPrivilegeModeV1 p,unsigned long long a,unsigned long long l,HybridCpuHostBufferAccessV1 x,std::vector<unsigned long long> args,std::wstring digest):ContextId(c),Service(s),Operation(o),PrivilegeMode(p),BufferAddress(a),BufferLength(l),BufferAccess(x),Arguments(std::make_shared<std::vector<unsigned long long>>(std::move(args))),TransitionDigest(std::move(digest)){} };
class HybridCpuExternalServiceResultV1 final { public: HybridCpuExternalServiceStatusV1 Status; unsigned long long ReturnValue; int ErrorCode; std::wstring Reason,ResultDigest; bool IsSuccess; HybridCpuExternalServiceResultV1(HybridCpuExternalServiceStatusV1 s,unsigned long long v,int e,std::wstring r,std::wstring d):Status(s),ReturnValue(v),ErrorCode(e),Reason(std::move(r)),ResultDigest(std::move(d)),IsSuccess(s==HybridCpuExternalServiceStatusV1::Success){} };
enum class HybridCpuVmProtectionV1 : unsigned char { None = 0, Read = 1, Write = 2, Execute = 4 };
struct HybridCpuVmRangeV1 final {
 unsigned long long Address, Size;
 HybridCpuVmProtectionV1 Protection;
 HybridCpuVmRangeV1(unsigned long long address, unsigned long long size, HybridCpuVmProtectionV1 protection)
  : Address(address), Size(size), Protection(protection) {}
};
class HybridCpuExecutionContextDescriptorV1 final { public: unsigned long long ContextId; int VirtualThreadCarrier; unsigned long long ContextCarrierAddress,EntryAddress,StackBase,StackSize,InitialStackPointer; std::shared_ptr<std::vector<std::shared_ptr<HybridCpuVmRangeV1>>> VmMappings; HybridCpuExecutionContextDescriptorV1(unsigned long long c,int v,unsigned long long ca,unsigned long long e,unsigned long long b,unsigned long long s,unsigned long long sp,std::shared_ptr<std::vector<std::shared_ptr<HybridCpuVmRangeV1>>> m):ContextId(c),VirtualThreadCarrier(v),ContextCarrierAddress(ca),EntryAddress(e),StackBase(b),StackSize(s),InitialStackPointer(sp),VmMappings(std::move(m)){} };
struct HybridCpuBootBlobServiceContractV1 final { static constexpr const wchar_t* SchemaId=L"hybridcpu.boot-blob-service/v1"; static constexpr unsigned long long GetOperation=1112297282ULL; static constexpr int MaximumBlobId=1024,MaximumBlobBytes=67108864; static constexpr const wchar_t* ContractDigest=L"f231a3f2120afb484921bcbf780aabad2f56118215ac19d247d5490824512610"; };
struct HybridCpuVirtualClockServiceContractV1 final {
 static constexpr const wchar_t* SchemaId=L"hybridcpu.virtual-clock-service/v1";
 static constexpr unsigned long long ReadTicksOperation=0x4d4f4e4fULL;
 static constexpr unsigned long long ReadDoomTicsOperation=0x444f4f4dULL;
 static constexpr unsigned long long WaitUntilDoomTicOperation=0x57414954ULL;
 static constexpr const wchar_t* ContractDigest=L"37ae233d9ebcff69b88bcaa7f6fa94ab6446c122729fe401f2a9c14b3cf31bf8";
};
struct HybridCpuConsoleServiceContractV1 final {
 static constexpr const wchar_t* SchemaId=L"hybridcpu.console-service/v1";
 static constexpr unsigned long long WriteUtf16Operation=0x43573136ULL;
 static constexpr unsigned long long SetTitleUtf16Operation=0x43543136ULL;
 static constexpr int MaximumCodeUnits=1024*1024;
 static constexpr int StringLengthOffsetBytes=16;
 static constexpr int StringDataOffsetBytes=20;
 static constexpr const wchar_t* ContractDigest=L"2a1ff4f3a364418889d17b100deb38316ea0ad57af98ce2909654c840aa5aea4";
};
struct HybridCpuManagedRuntimeEcallContractV1 final {
 static constexpr const wchar_t* SchemaId=L"hybridcpu.managed-runtime-ecall/v1";
 static constexpr unsigned long long ArgumentExceptionGetMessageOperation=1, EnsureTypeInitializedOperation=2,
  StaticStoreInt32Operation=3, StaticLoadInt32Operation=4, AllocateNullReferenceExceptionOperation=5,
  ArrayStoreInt32Operation=6, ArrayStoreReferenceOperation=7, InitializeArrayOperation=8, NewArrayOperation=9,
  StaticStoreReferenceOperation=10, StaticLoadReferenceOperation=11, AllocateObjectOperation=12,
  ArrayLoadReferenceOperation=13, DivideUInt32CheckedOperation=14, ArrayLoadInt32Operation=15,
  ArrayEmptyOperation=16, ArrayLengthOperation=17, ArrayStoreInt8Operation=18, StringCharacterOperation=19,
  StringLengthOperation=20, StringConcat2Operation=21, ArrayLoadUInt8Operation=22,
  DivideInt32CheckedOperation=23, DivideInt64CheckedOperation=24, MathAbsInt64CheckedOperation=25,
  ArrayLoadInt16Operation=26, ArrayLoadUInt16Operation=27, ArrayCopyAllOperation=28, IsInstanceOperation=29,
  RemainderInt32CheckedOperation=30, StringConcat3Operation=31, ArrayStoreInt16Operation=32,
  StringFromUtf16ArrayOperation=33, ArrayCopyOperation=34, ArgumentNullCtorParamNameOperation=35,
  ArgumentOutOfRangeCtorParamNameOperation=36, StringNotEqualsOperation=37, ArrayClearOperation=38,
  ArrayLoadInt64Operation=39, NullableValueInt64CheckedOperation=40;
 static constexpr int ArrayCopyArgumentBlockBytes=40;
};
struct HybridCpuExternalServiceEcallContractV1 final { static constexpr const wchar_t* SchemaId=L"hybridcpu.external-service-ecall/v1"; static constexpr unsigned long long EcallNumber=1212371798ULL; static constexpr int EcallNumberRegister=17,ServiceRegister=10,OperationRegister=11,BufferAddressRegister=12,BufferLengthRegister=13,BufferAccessRegister=14,ArgumentCountRegister=15,FirstArgumentRegister=16,SecondArgumentRegister=18,ThirdArgumentRegister=19,MaximumRegisterArguments=3,ResultValueRegister=10,ResultStatusRegister=11,ResultErrorRegister=12; static constexpr const wchar_t* ContractDigest=L"01fa0c4f54856d784df5421e86d0badbc6c95fc4c5b45ca5bbfeed54a4e03b96"; };
}
namespace HybridCPU::RuntimeKernel { class IHybridCpuRuntimeKernelV1; }
namespace HybridCPU::ManagedRuntime {
enum class HybridCpuManagedHeapStatusV1 : unsigned char { Success=0,NotInitialized=1,InvalidConfiguration=2,KernelFailure=3,InvalidTypeDescriptor=4,ObjectTooLarge=5,SizeOverflow=6,OutOfMemory=7,InvalidObjectAccess=8 };
enum class HybridCpuManagedGcRootSourceV1 : unsigned char { Static=0,StringLiteral=1,Handle=2,Pinned=3,HelperTemporary=4,ThreadStatic=5 };
struct HybridCpuManagedGcRootV1 final { std::wstring Identity; HybridCpuManagedGcRootSourceV1 Source; unsigned long long ObjectReference; HybridCpuManagedGcRootV1(std::wstring i,HybridCpuManagedGcRootSourceV1 s,unsigned long long r):Identity(std::move(i)),Source(s),ObjectReference(r){} };
class HybridCpuManagedGcRootRegistryV1 final {
 std::map<std::wstring,std::shared_ptr<HybridCpuManagedGcRootV1>> _roots;
public:
 bool Register(const std::wstring& id,HybridCpuManagedGcRootSourceV1 source,unsigned long long ref){if(id.empty()||!ref)return false;auto i=_roots.find(id);if(i!=_roots.end())return i->second&&i->second->Source==source&&i->second->ObjectReference==ref;_roots.emplace(id,std::make_shared<HybridCpuManagedGcRootV1>(id,source,ref));return true;}
 bool Unregister(const std::wstring& id,HybridCpuManagedGcRootSourceV1 source){auto i=_roots.find(id);if(i==_roots.end()||!i->second||i->second->Source!=source)return false;_roots.erase(i);return true;}
 std::vector<std::shared_ptr<HybridCpuManagedGcRootV1>> Snapshot()const{std::vector<std::shared_ptr<HybridCpuManagedGcRootV1>> r;r.reserve(_roots.size());for(const auto& item:_roots)r.push_back(item.second);return r;}
};
class HybridCpuManagedTypeSystemV1 final {
 std::unordered_map<unsigned long long,std::shared_ptr<HybridCPU::Platform::Contracts::HybridCpuManagedTypeDescriptorV1>> _types,_handles;
 std::unordered_map<unsigned long long,unsigned long long> _handlesByTypeId;
 std::unordered_map<unsigned long long,std::vector<unsigned char>> _staticStorage;
 std::unordered_map<unsigned long long,HybridCPU::Platform::Contracts::HybridCpuManagedTypeInitializationStateV1> _initialization;
public:
 std::vector<std::shared_ptr<HybridCPU::Platform::Contracts::HybridCpuManagedTypeDescriptorV1>> Descriptors; std::wstring Digest;
 bool HasExecutionAuthority=false,HasObjectLifetimeAuthority=false,HasGcAuthority=false;
 HybridCpuManagedTypeSystemV1(std::vector<std::shared_ptr<HybridCPU::Platform::Contracts::HybridCpuManagedTypeDescriptorV1>> descriptors,std::wstring digest,std::unordered_map<unsigned long long,unsigned long long> handles)
 :_handlesByTypeId(std::move(handles)),Descriptors(std::move(descriptors)),Digest(std::move(digest)){for(const auto& t:Descriptors){if(!t||!t->TypeId)continue;_types.emplace(t->TypeId,t);_initialization.emplace(t->TypeId,HybridCPU::Platform::Contracts::HybridCpuManagedTypeInitializationStateV1::Uninitialized);auto h=_handlesByTypeId.find(t->TypeId);if(h!=_handlesByTypeId.end()&&h->second)_handles.emplace(h->second,t);}}
 std::shared_ptr<HybridCPU::Platform::Contracts::HybridCpuManagedTypeDescriptorV1> Resolve(unsigned long long id)const{auto i=_types.find(id);return i==_types.end()?nullptr:i->second;}
 std::shared_ptr<HybridCPU::Platform::Contracts::HybridCpuManagedTypeDescriptorV1> ResolveTypeHandle(unsigned long long h)const{auto i=_handles.find(h);return i==_handles.end()?nullptr:i->second;}
 std::optional<unsigned long long> TypeHandle(unsigned long long id)const{auto i=_handlesByTypeId.find(id);return i==_handlesByTypeId.end()||!i->second?std::nullopt:std::optional<unsigned long long>(i->second);}
 bool IsAssignable(unsigned long long from,unsigned long long to)const{if(from==to)return true;std::unordered_set<unsigned long long> seen;std::vector<unsigned long long> todo{from};while(!todo.empty()){auto id=todo.back();todo.pop_back();if(!seen.insert(id).second)continue;auto t=Resolve(id);if(!t)continue;if(t->BaseTypeId){if(*t->BaseTypeId==to)return true;todo.push_back(*t->BaseTypeId);}if(t->InterfaceTypeIds)for(auto i:*t->InterfaceTypeIds){if(i==to)return true;todo.push_back(i);}}return false;}
 std::vector<unsigned char> StaticStorage(unsigned long long id){auto t=Resolve(id);if(!t||!t->StaticLayout||t->StaticLayout->SizeBytes<0)return {};auto [i,added]=_staticStorage.try_emplace(id,static_cast<size_t>(t->StaticLayout->SizeBytes),0);return i->second;}
 bool TryStoreStaticReference(unsigned long long handle,int offset,unsigned long long value){
  auto type=ResolveTypeHandle(handle);if(!type||!type->StaticLayout||type->StaticLayout->SizeBytes<8||!type->StaticLayout->Fields||offset<0)return false;
  auto field=std::find_if(type->StaticLayout->Fields->begin(),type->StaticLayout->Fields->end(),[&](const auto& row){return row&&row->OffsetBytes==offset;});
  if(field==type->StaticLayout->Fields->end()||(*field)->StorageKind!=HybridCPU::Platform::Contracts::HybridCpuManagedStorageKindV1::ObjectReference||(*field)->SizeBytes!=8||offset>type->StaticLayout->SizeBytes-8)return false;
  auto [storage,added]=_staticStorage.try_emplace(type->TypeId,static_cast<size_t>(type->StaticLayout->SizeBytes),0);
  (void)added;if(storage->second.size()!=static_cast<size_t>(type->StaticLayout->SizeBytes))return false;
  for(size_t i=0;i<sizeof(value);++i)storage->second[static_cast<size_t>(offset)+i]=static_cast<unsigned char>((value>>(i*8))&0xffu);
  return true;
 }
 HybridCPU::Platform::Contracts::HybridCpuManagedTypeInitializationStateV1 InitializationState(unsigned long long id)const{auto i=_initialization.find(id);return i==_initialization.end()?HybridCPU::Platform::Contracts::HybridCpuManagedTypeInitializationStateV1::Failed:i->second;}
 bool TryBeginInitialization(unsigned long long id){auto i=_initialization.find(id);if(i==_initialization.end()||i->second!=HybridCPU::Platform::Contracts::HybridCpuManagedTypeInitializationStateV1::Uninitialized)return false;i->second=HybridCPU::Platform::Contracts::HybridCpuManagedTypeInitializationStateV1::Running;return true;}
 bool TryCompleteInitialization(unsigned long long id,bool ok){auto i=_initialization.find(id);if(i==_initialization.end()||i->second!=HybridCPU::Platform::Contracts::HybridCpuManagedTypeInitializationStateV1::Running)return false;i->second=ok?HybridCPU::Platform::Contracts::HybridCpuManagedTypeInitializationStateV1::Initialized:HybridCPU::Platform::Contracts::HybridCpuManagedTypeInitializationStateV1::Failed;return true;}
};
struct HybridCpuManagedHeapOptionsV1 final { unsigned long long BaseAddress,SizeBytes; int MaximumObjectSizeBytes,OutOfMemoryExitCode; std::wstring OptionsDigest; static constexpr unsigned long long DefaultBaseAddress=1073741824ULL,DefaultSizeBytes=16777216ULL; static constexpr int DefaultMaximumObjectSizeBytes=1048576,DefaultOutOfMemoryExitCode=-3; HybridCpuManagedHeapOptionsV1(unsigned long long b,unsigned long long s,int m,int e,std::wstring d):BaseAddress(b),SizeBytes(s),MaximumObjectSizeBytes(m),OutOfMemoryExitCode(e),OptionsDigest(std::move(d)){} static std::shared_ptr<HybridCpuManagedHeapOptionsV1> Production(){return std::make_shared<HybridCpuManagedHeapOptionsV1>(DefaultBaseAddress,DefaultSizeBytes,DefaultMaximumObjectSizeBytes,DefaultOutOfMemoryExitCode,L"b21c99b5ccda568cade901958cc8f8ff3d8e99a0a6d4c64cf588c9dba4790af9");} static std::shared_ptr<HybridCpuManagedHeapOptionsV1> Create(unsigned long long,unsigned long long,int,int); };
struct HybridCpuManagedHeapResultV1 final { HybridCpuManagedHeapStatusV1 Status; std::wstring Reason; unsigned long long ObjectReference; int ObjectSizeBytes; std::wstring ResultDigest; bool IsSuccess; HybridCpuManagedHeapResultV1(HybridCpuManagedHeapStatusV1 s,std::wstring r,unsigned long long ref,int size,std::wstring d):Status(s),Reason(std::move(r)),ObjectReference(ref),ObjectSizeBytes(size),ResultDigest(std::move(d)),IsSuccess(s==HybridCpuManagedHeapStatusV1::Success){} };
struct HybridCpuManagedActiveAllocationV1 final { unsigned long long ObjectAddress; int ObjectSizeBytes; unsigned long long TypeId; std::wstring TypeDescriptorDigest; HybridCpuManagedActiveAllocationV1(unsigned long long a,int s,unsigned long long t,std::wstring d):ObjectAddress(a),ObjectSizeBytes(s),TypeId(t),TypeDescriptorDigest(std::move(d)){} };
class IHybridCpuManagedHeapMemoryV1 { public: virtual ~IHybridCpuManagedHeapMemoryV1()=default; virtual unsigned long long BaseAddress()const=0; virtual unsigned long long SizeBytes()const=0; virtual bool Clear(unsigned long long,int)=0; virtual bool Read(unsigned long long,std::span<unsigned char>)const=0; virtual bool Write(unsigned long long,std::span<const unsigned char>)=0; };
class HybridCpuManagedHeapAllocatorV1 final {
 std::shared_ptr<HybridCPU::RuntimeKernel::IHybridCpuRuntimeKernelV1> _kernel; std::shared_ptr<HybridCpuManagedTypeSystemV1> _types; std::shared_ptr<HybridCpuManagedHeapOptionsV1> _options; std::shared_ptr<IHybridCpuManagedHeapMemoryV1> _memory; std::map<unsigned long long,std::shared_ptr<HybridCpuManagedActiveAllocationV1>> _active; unsigned long long _next=0; bool _initialized=false;
public:
 HybridCpuManagedHeapAllocatorV1(std::shared_ptr<HybridCPU::RuntimeKernel::IHybridCpuRuntimeKernelV1> k,std::shared_ptr<HybridCpuManagedTypeSystemV1> t,std::shared_ptr<HybridCpuManagedHeapOptionsV1> o,std::shared_ptr<IHybridCpuManagedHeapMemoryV1> m):_kernel(std::move(k)),_types(std::move(t)),_options(std::move(o)),_memory(std::move(m)){}
 HybridCpuManagedHeapResultV1 Initialize(){if(!_kernel||!_types||!_options||!_memory||!_options->SizeBytes||_options->MaximumObjectSizeBytes<=0||_options->BaseAddress<_memory->BaseAddress()||_options->BaseAddress-_memory->BaseAddress()>_memory->SizeBytes()||_options->SizeBytes>_memory->SizeBytes()-(_options->BaseAddress-_memory->BaseAddress()))return {HybridCpuManagedHeapStatusV1::InvalidConfiguration,L"Managed heap memory or options are invalid.",0,0,L""};_next=_options->BaseAddress;_initialized=true;return {HybridCpuManagedHeapStatusV1::Success,L"",0,0,L""};}
 std::shared_ptr<HybridCpuManagedHeapResultV1> TryAllocateVariable(unsigned long long handle,int size){if(!_initialized||!_types||!_memory)return std::make_shared<HybridCpuManagedHeapResultV1>(HybridCpuManagedHeapStatusV1::NotInitialized,L"Managed heap is not initialized.",0,0,L"");if(size<=0)return std::make_shared<HybridCpuManagedHeapResultV1>(HybridCpuManagedHeapStatusV1::SizeOverflow,L"Object size must be positive.",0,0,L"");if(size>_options->MaximumObjectSizeBytes)return std::make_shared<HybridCpuManagedHeapResultV1>(HybridCpuManagedHeapStatusV1::ObjectTooLarge,L"Object exceeds the configured maximum size.",0,0,L"");auto t=_types->ResolveTypeHandle(handle);if(!t||t->Kind!=HybridCPU::Platform::Contracts::HybridCpuManagedTypeKindV1::SzArray)return std::make_shared<HybridCpuManagedHeapResultV1>(HybridCpuManagedHeapStatusV1::InvalidTypeDescriptor,L"Array type handle is invalid.",0,0,L"");const auto limit=_options->BaseAddress+_options->SizeBytes;if(limit<_options->BaseAddress||_next>limit||static_cast<unsigned long long>(size)>limit-_next)return std::make_shared<HybridCpuManagedHeapResultV1>(HybridCpuManagedHeapStatusV1::OutOfMemory,L"Managed heap capacity is exhausted.",0,0,L"");auto address=_next;if(!_memory->Clear(address,size))return std::make_shared<HybridCpuManagedHeapResultV1>(HybridCpuManagedHeapStatusV1::KernelFailure,L"Managed heap memory clear failed.",0,0,L"");_next+=static_cast<unsigned long long>(size);_active.emplace(address,std::make_shared<HybridCpuManagedActiveAllocationV1>(address,size,t->TypeId,t->DescriptorDigest));return std::make_shared<HybridCpuManagedHeapResultV1>(HybridCpuManagedHeapStatusV1::Success,L"",address,size,L"");}
 std::shared_ptr<HybridCpuManagedHeapResultV1> TryAllocateStringVariable(unsigned long long handle,int size){
  using namespace HybridCPU::Platform::Contracts;
  const auto failure=[](HybridCpuManagedHeapStatusV1 status,const wchar_t* reason){return std::make_shared<HybridCpuManagedHeapResultV1>(status,reason,0,0,L"");};
  if(!_initialized||!_types||!_memory||!_options)return failure(HybridCpuManagedHeapStatusV1::NotInitialized,L"Managed heap is not initialized.");
  if(size<=0)return failure(HybridCpuManagedHeapStatusV1::SizeOverflow,L"Object size must be positive.");
  auto type=_types->ResolveTypeHandle(handle);
  if(!type||type->Kind!=HybridCpuManagedTypeKindV1::String||size<16||size<type->InstanceSizeBytes)return failure(HybridCpuManagedHeapStatusV1::InvalidTypeDescriptor,L"Variable string allocation requires an exact string type handle and fixed prefix.");
  constexpr unsigned long long alignment=8;
  if(static_cast<unsigned long long>(size)>std::numeric_limits<unsigned long long>::max()-(alignment-1))return failure(HybridCpuManagedHeapStatusV1::SizeOverflow,L"Aligned string object size overflowed.");
  const unsigned long long aligned=(static_cast<unsigned long long>(size)+(alignment-1))&~(alignment-1);
  if(aligned>static_cast<unsigned long long>(_options->MaximumObjectSizeBytes))return failure(HybridCpuManagedHeapStatusV1::ObjectTooLarge,L"String object exceeds the configured maximum size.");
  if(_next>std::numeric_limits<unsigned long long>::max()-(alignment-1))return failure(HybridCpuManagedHeapStatusV1::SizeOverflow,L"Aligned heap address overflowed.");
  const unsigned long long address=(_next+(alignment-1))&~(alignment-1);
  const unsigned long long limit=_options->BaseAddress+_options->SizeBytes;
  if(limit<_options->BaseAddress||address<_options->BaseAddress||address>limit||aligned>limit-address)return failure(HybridCpuManagedHeapStatusV1::OutOfMemory,L"Managed heap capacity is exhausted.");
  _next=address+aligned;
  const int allocationSize=static_cast<int>(aligned);
  if(!_memory->Clear(address,allocationSize))return failure(HybridCpuManagedHeapStatusV1::InvalidObjectAccess,L"Managed heap backing rejected allocation zeroing.");
  std::array<unsigned char,16> header{};
  for(std::size_t i=0;i<sizeof(handle);++i)header[i]=static_cast<unsigned char>((handle>>(i*8))&0xffu);
  if(!_memory->Write(address,std::span<const unsigned char>(header)))return failure(HybridCpuManagedHeapStatusV1::InvalidObjectAccess,L"Managed heap backing rejected object-header initialization.");
  _active.emplace(address,std::make_shared<HybridCpuManagedActiveAllocationV1>(address,allocationSize,type->TypeId,type->DescriptorDigest));
  return std::make_shared<HybridCpuManagedHeapResultV1>(HybridCpuManagedHeapStatusV1::Success,L"",address,allocationSize,L"");
 }
 bool TryGetAllocation(unsigned long long a,HybridCpuManagedActiveAllocationV1& out)const{auto i=_active.find(a);if(i==_active.end()||!i->second)return false;out=*i->second;return true;}
 bool TryReadObjectBytes(unsigned long long a,int off,std::span<unsigned char> b)const{HybridCpuManagedActiveAllocationV1 x(a,0,0,L"");if(!_initialized||!_memory||off<0||!TryGetAllocation(a,x)||off>x.ObjectSizeBytes||b.size()>static_cast<size_t>(x.ObjectSizeBytes-off))return false;return _memory->Read(a+static_cast<unsigned long long>(off),b);}
 HybridCpuManagedHeapResultV1 WriteObjectBytes(unsigned long long a,int off,std::span<const unsigned char> b){HybridCpuManagedActiveAllocationV1 x(a,0,0,L"");if(!_initialized||!_memory||off<0||!TryGetAllocation(a,x)||off>x.ObjectSizeBytes||b.size()>static_cast<size_t>(x.ObjectSizeBytes-off)||!_memory->Write(a+static_cast<unsigned long long>(off),b))return {HybridCpuManagedHeapStatusV1::InvalidObjectAccess,L"Managed heap object write is outside a live allocation.",0,0,L""};return {HybridCpuManagedHeapStatusV1::Success,L"",a,x.ObjectSizeBytes,L""};}
};
}
namespace HybridCPU::RuntimeKernel {
class HybridCpuKernelResultV1 final {
public:
 HybridCPU::Platform::Contracts::HybridCpuKernelStatusV1 Status; std::wstring Reason; bool IsSuccess;
 HybridCpuKernelResultV1(HybridCPU::Platform::Contracts::HybridCpuKernelStatusV1 status,std::wstring reason)
  :Status(status),Reason(std::move(reason)),IsSuccess(status==HybridCPU::Platform::Contracts::HybridCpuKernelStatusV1::Success){}
};
class HybridCpuManagedBootBlobProviderResultV1 final { public: bool Success; unsigned long long ManagedReference; int Length; std::wstring RegistryDigest,Reason; HybridCpuManagedBootBlobProviderResultV1(bool s,unsigned long long r,int n,std::wstring d,std::wstring why):Success(s),ManagedReference(r),Length(n),RegistryDigest(std::move(d)),Reason(std::move(why)){} };
class HybridCpuHostProviderResultV1 final {
public:
 HybridCPU::Platform::Contracts::HybridCpuExternalServiceStatusV1 Status;
 unsigned long long ReturnValue;
 int ErrorCode;
 std::wstring Reason;
 HybridCpuHostProviderResultV1(HybridCPU::Platform::Contracts::HybridCpuExternalServiceStatusV1 status,unsigned long long returnValue,int errorCode,std::wstring reason)
  :Status(status),ReturnValue(returnValue),ErrorCode(errorCode),Reason(std::move(reason)){}
};
class IHybridCpuHostServiceProviderV1 {
public:
 virtual ~IHybridCpuHostServiceProviderV1()=default;
 virtual std::shared_ptr<HybridCpuHostProviderResultV1> Invoke(std::shared_ptr<HybridCPU::Platform::Contracts::HybridCpuExternalServiceRequestV1> request)=0;
};
class IHybridCpuManagedBootBlobProviderV1 { public: virtual ~IHybridCpuManagedBootBlobProviderV1()=default; virtual std::shared_ptr<HybridCpuManagedBootBlobProviderResultV1> Resolve(int)=0; };
class IHybridCpuRuntimeKernelV1 { public: virtual ~IHybridCpuRuntimeKernelV1()=default; virtual std::shared_ptr<HybridCPU::Platform::Contracts::HybridCpuExecutionContextDescriptorV1> CurrentContext()=0; virtual std::shared_ptr<HybridCPU::Platform::Contracts::HybridCpuExternalServiceResultV1> ExternalServiceTransition(const HybridCPU::Platform::Contracts::HybridCpuExternalServiceRequestV1&)=0; virtual std::shared_ptr<HybridCpuKernelResultV1> ProcessExit(int)=0; };
}
namespace HybridCPU::Platform::Contracts {
struct HybridCpuManagedInteropContractV1 final {
 static std::wstring ComputeTransitionDigest(const std::shared_ptr<HybridCpuExternalServiceRequestV1>& request){
  if(!request)return L"";
  const auto service=[](HybridCpuHostServiceV1 v)->std::wstring{switch(v){case HybridCpuHostServiceV1::ProcessExit:return L"ProcessExit";case HybridCpuHostServiceV1::Console:return L"Console";case HybridCpuHostServiceV1::File:return L"File";case HybridCpuHostServiceV1::Network:return L"Network";case HybridCpuHostServiceV1::Clock:return L"Clock";case HybridCpuHostServiceV1::Graphics:return L"Graphics";case HybridCpuHostServiceV1::Input:return L"Input";case HybridCpuHostServiceV1::ManagedRuntime:return L"ManagedRuntime";}return L"";};
  const auto privilege=[](HybridCpuPrivilegeModeV1 v)->std::wstring{switch(v){case HybridCpuPrivilegeModeV1::User:return L"User";case HybridCpuPrivilegeModeV1::Supervisor:return L"Supervisor";case HybridCpuPrivilegeModeV1::Machine:return L"Machine";}return L"";};
  const auto access=[](HybridCpuHostBufferAccessV1 v)->std::wstring{switch(v){case HybridCpuHostBufferAccessV1::None:return L"None";case HybridCpuHostBufferAccessV1::Read:return L"Read";case HybridCpuHostBufferAccessV1::Write:return L"Write";case HybridCpuHostBufferAccessV1::ReadWrite:return L"ReadWrite";}return L"";};
  std::wstring args; if(request->Arguments)for(size_t i=0;i<request->Arguments->size();++i){if(i)args.push_back(static_cast<wchar_t>(44));args+=std::to_wstring((*request->Arguments)[i]);}
  const std::wstring canonical=std::wstring(L"195c7bcb9612b897fe8165704a6bd828e5cab1be7da03fab299bcd62df1361b5")+L"|"+std::to_wstring(request->ContextId)+L"|"+service(request->Service)+L"|"+std::to_wstring(request->Operation)+L"|"+privilege(request->PrivilegeMode)+L"|"+std::to_wstring(request->BufferAddress)+L"|"+std::to_wstring(request->BufferLength)+L"|"+access(request->BufferAccess)+L"|"+args;
  return HybridCPU::ManagedRuntime::HybridCpuManagedGcRuntimeContractV1::Hash(canonical);
 }
};
}
namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed {
enum class HybridCpuIseManagedImageLoadStatusV1 : unsigned char { Success=0,InvalidImage=1,MemoryRejected=2,KernelRejected=3,TypeMetadataRejected=4,HeapRejected=5,BootstrapRejected=6,GcMetadataRejected=7 };
class HybridCpuIseManagedImageLoadResultV1 final {
public:
 HybridCpuIseManagedImageLoadStatusV1 Status; std::wstring Reason;
 std::shared_ptr<HybridCPU::Platform::Contracts::HybridCpuExecutionContextDescriptorV1> Context;
 std::shared_ptr<HybridCPU::ManagedRuntime::HybridCpuManagedTypeSystemV1> TypeSystem;
 std::shared_ptr<HybridCPU::ManagedRuntime::HybridCpuManagedHeapAllocatorV1> Heap;
 std::shared_ptr<HybridCPU::ManagedRuntime::HybridCpuManagedStringRuntimeV1> Strings; std::shared_ptr<HybridCPU::ManagedRuntime::HybridCpuManagedNonMovingGcV1> Gc;
 std::shared_ptr<HybridCpuManagedBootstrapResultV1> Bootstrap; std::shared_ptr<YAKSys_Hybrid_CPU::Core::IHybridCpuIseEcallBridgeV1> EcallBridge;
 std::shared_ptr<std::vector<std::shared_ptr<HybridCpuManagedStackMapRegistrationV1>>> StackMaps;
 std::shared_ptr<std::vector<std::shared_ptr<HybridCpuIseManagedInitializerBindingV1>>> PendingInitializers;
 std::shared_ptr<std::vector<std::shared_ptr<HybridCPU::ManagedRuntime::HybridCpuManagedGcRootV1>>> ProcessRoots; bool IsSuccess;
 HybridCpuIseManagedImageLoadResultV1(HybridCpuIseManagedImageLoadStatusV1 s,std::wstring r,std::shared_ptr<HybridCPU::Platform::Contracts::HybridCpuExecutionContextDescriptorV1> c,
 std::shared_ptr<HybridCPU::ManagedRuntime::HybridCpuManagedTypeSystemV1> t,std::shared_ptr<HybridCPU::ManagedRuntime::HybridCpuManagedHeapAllocatorV1> h,
 std::shared_ptr<HybridCPU::ManagedRuntime::HybridCpuManagedStringRuntimeV1> strings,std::shared_ptr<HybridCPU::ManagedRuntime::HybridCpuManagedNonMovingGcV1> gc,std::shared_ptr<HybridCpuManagedBootstrapResultV1> bootstrap,
 std::shared_ptr<YAKSys_Hybrid_CPU::Core::IHybridCpuIseEcallBridgeV1> bridge=nullptr,std::shared_ptr<std::vector<std::shared_ptr<HybridCpuManagedStackMapRegistrationV1>>> maps=nullptr,
 std::shared_ptr<std::vector<std::shared_ptr<HybridCpuIseManagedInitializerBindingV1>>> init=nullptr,
 std::shared_ptr<std::vector<std::shared_ptr<HybridCPU::ManagedRuntime::HybridCpuManagedGcRootV1>>> roots=nullptr)
 :Status(s),Reason(std::move(r)),Context(std::move(c)),TypeSystem(std::move(t)),Heap(std::move(h)),Strings(std::move(strings)),Gc(std::move(gc)),Bootstrap(std::move(bootstrap)),EcallBridge(std::move(bridge)),StackMaps(std::move(maps)),PendingInitializers(std::move(init)),ProcessRoots(std::move(roots)),IsSuccess(s==HybridCpuIseManagedImageLoadStatusV1::Success){}
 bool HasImageMaterializationAuthority()const{return IsSuccess;} bool HasRuntimeBootstrapAuthority()const{return IsSuccess;} bool HasExecutionAuthority()const{return false;} bool HasEcallAuthority()const{return false;}
};
}
namespace HybridCPU::ManagedRuntime {
class HybridCpuManagedBootBlobRegistryV1 final {
 struct Entry { unsigned long long Reference; int Length; std::wstring Digest; };
 std::shared_ptr<HybridCpuManagedTypeSystemV1> _types; std::shared_ptr<HybridCpuManagedHeapAllocatorV1> _heap; std::shared_ptr<HybridCpuManagedGcRootRegistryV1> _roots; std::map<int,Entry> _blobs;
 bool Inspect(int id,unsigned long long ref,HybridCpuManagedActiveAllocationV1& allocation,int& length,std::wstring& digest)const{
  if(!_types||!_heap||!_roots||id<=0||id>HybridCPU::Platform::Contracts::HybridCpuBootBlobServiceContractV1::MaximumBlobId||!ref||!_heap->TryGetAllocation(ref,allocation))return false;
  auto type=_types->Resolve(allocation.TypeId);if(!type||type->Kind!=HybridCPU::Platform::Contracts::HybridCpuManagedTypeKindV1::SzArray||!type->ArrayShape||!type->ArrayShape->IsSzArray||type->ArrayShape->ElementStorageKind!=HybridCPU::Platform::Contracts::HybridCpuManagedStorageKindV1::Primitive||type->ArrayShape->ElementSizeBytes!=1||type->ArrayShape->ElementAlignmentBytes!=1||allocation.ObjectSizeBytes<4)return false;
  const auto& shape=*type->ArrayShape;if(shape.LengthOffsetBytes<0||shape.LengthOffsetBytes>allocation.ObjectSizeBytes-4||shape.DataOffsetBytes<shape.LengthOffsetBytes+4||shape.DataOffsetBytes>allocation.ObjectSizeBytes)return false;
  std::array<unsigned char,4> raw{};if(!_heap->TryReadObjectBytes(ref,shape.LengthOffsetBytes,std::span<unsigned char>(raw)))return false;
  const unsigned int n=static_cast<unsigned int>(raw[0])|(static_cast<unsigned int>(raw[1])<<8)|(static_cast<unsigned int>(raw[2])<<16)|(static_cast<unsigned int>(raw[3])<<24);
  if(!n||n>static_cast<unsigned int>(HybridCPU::Platform::Contracts::HybridCpuBootBlobServiceContractV1::MaximumBlobBytes)||static_cast<unsigned long long>(shape.DataOffsetBytes)>static_cast<unsigned long long>(allocation.ObjectSizeBytes)-n)return false;
  length=static_cast<int>(n);const std::wstring canonical=std::wstring(HybridCPU::Platform::Contracts::HybridCpuBootBlobServiceContractV1::ContractDigest)+L"|"+std::to_wstring(id)+L"|"+std::to_wstring(ref)+L"|"+std::to_wstring(length)+L"|"+std::to_wstring(allocation.TypeId)+L"|"+type->DescriptorDigest;
  digest=HybridCpuManagedGcRuntimeContractV1::Hash(canonical);return !digest.empty();
 }
public:
 HybridCpuManagedBootBlobRegistryV1(std::shared_ptr<HybridCpuManagedTypeSystemV1> t,std::shared_ptr<HybridCpuManagedHeapAllocatorV1> h,std::shared_ptr<HybridCpuManagedGcRootRegistryV1> r):_types(std::move(t)),_heap(std::move(h)),_roots(std::move(r)){}
 bool Register(int id,unsigned long long ref){HybridCpuManagedActiveAllocationV1 a(ref,0,0,L"");int length=0;std::wstring digest;if(!Inspect(id,ref,a,length,digest))return false;auto old=_blobs.find(id);if(old!=_blobs.end())return old->second.Reference==ref&&old->second.Length==length;const auto root=L"boot-blob:"+std::to_wstring(id);if(!_roots->Register(root,HybridCpuManagedGcRootSourceV1::Handle,ref))return false;_blobs.emplace(id,Entry{ref,length,std::move(digest)});return true;}
 std::shared_ptr<HybridCPU::RuntimeKernel::HybridCpuManagedBootBlobProviderResultV1> Resolve(int id)const{auto i=_blobs.find(id);if(i==_blobs.end())return std::make_shared<HybridCPU::RuntimeKernel::HybridCpuManagedBootBlobProviderResultV1>(false,0,0,L"",L"The requested immutable boot blob is not registered.");return std::make_shared<HybridCPU::RuntimeKernel::HybridCpuManagedBootBlobProviderResultV1>(true,i->second.Reference,i->second.Length,i->second.Digest,L"");}
};
}
