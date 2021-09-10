// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: workflow.proto

#include "workflow.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG
namespace SmartPeakServer {
constexpr WorkflowParameters::WorkflowParameters(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : sequence_file_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string){}
struct WorkflowParametersDefaultTypeInternal {
  constexpr WorkflowParametersDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~WorkflowParametersDefaultTypeInternal() {}
  union {
    WorkflowParameters _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT WorkflowParametersDefaultTypeInternal _WorkflowParameters_default_instance_;
constexpr WorkflowStatus::WorkflowStatus(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : status_code_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , session_id_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , path_to_results_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , progress_in_percent_(0){}
struct WorkflowStatusDefaultTypeInternal {
  constexpr WorkflowStatusDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~WorkflowStatusDefaultTypeInternal() {}
  union {
    WorkflowStatus _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT WorkflowStatusDefaultTypeInternal _WorkflowStatus_default_instance_;
}  // namespace SmartPeakServer
static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_workflow_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_workflow_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_workflow_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_workflow_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::SmartPeakServer::WorkflowParameters, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::SmartPeakServer::WorkflowParameters, sequence_file_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::SmartPeakServer::WorkflowStatus, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::SmartPeakServer::WorkflowStatus, progress_in_percent_),
  PROTOBUF_FIELD_OFFSET(::SmartPeakServer::WorkflowStatus, status_code_),
  PROTOBUF_FIELD_OFFSET(::SmartPeakServer::WorkflowStatus, session_id_),
  PROTOBUF_FIELD_OFFSET(::SmartPeakServer::WorkflowStatus, path_to_results_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::SmartPeakServer::WorkflowParameters)},
  { 6, -1, sizeof(::SmartPeakServer::WorkflowStatus)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::SmartPeakServer::_WorkflowParameters_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::SmartPeakServer::_WorkflowStatus_default_instance_),
};

const char descriptor_table_protodef_workflow_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\016workflow.proto\022\017SmartPeakServer\"+\n\022Wor"
  "kflowParameters\022\025\n\rsequence_file\030\001 \001(\t\"o"
  "\n\016WorkflowStatus\022\033\n\023progress_in_percent\030"
  "\001 \001(\005\022\023\n\013status_code\030\002 \001(\t\022\022\n\nsession_id"
  "\030\003 \001(\t\022\027\n\017path_to_results\030\004 \001(\t2a\n\010Workf"
  "low\022U\n\013runWorkflow\022#.SmartPeakServer.Wor"
  "kflowParameters\032\037.SmartPeakServer.Workfl"
  "owStatus\"\000b\006proto3"
  ;
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_workflow_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_workflow_2eproto = {
  false, false, 298, descriptor_table_protodef_workflow_2eproto, "workflow.proto", 
  &descriptor_table_workflow_2eproto_once, nullptr, 0, 2,
  schemas, file_default_instances, TableStruct_workflow_2eproto::offsets,
  file_level_metadata_workflow_2eproto, file_level_enum_descriptors_workflow_2eproto, file_level_service_descriptors_workflow_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK ::PROTOBUF_NAMESPACE_ID::Metadata
descriptor_table_workflow_2eproto_metadata_getter(int index) {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_workflow_2eproto);
  return descriptor_table_workflow_2eproto.file_level_metadata[index];
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_workflow_2eproto(&descriptor_table_workflow_2eproto);
namespace SmartPeakServer {

// ===================================================================

class WorkflowParameters::_Internal {
 public:
};

WorkflowParameters::WorkflowParameters(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:SmartPeakServer.WorkflowParameters)
}
WorkflowParameters::WorkflowParameters(const WorkflowParameters& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  sequence_file_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_sequence_file().empty()) {
    sequence_file_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_sequence_file(), 
      GetArena());
  }
  // @@protoc_insertion_point(copy_constructor:SmartPeakServer.WorkflowParameters)
}

void WorkflowParameters::SharedCtor() {
sequence_file_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

WorkflowParameters::~WorkflowParameters() {
  // @@protoc_insertion_point(destructor:SmartPeakServer.WorkflowParameters)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void WorkflowParameters::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  sequence_file_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void WorkflowParameters::ArenaDtor(void* object) {
  WorkflowParameters* _this = reinterpret_cast< WorkflowParameters* >(object);
  (void)_this;
}
void WorkflowParameters::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void WorkflowParameters::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void WorkflowParameters::Clear() {
// @@protoc_insertion_point(message_clear_start:SmartPeakServer.WorkflowParameters)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  sequence_file_.ClearToEmpty();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* WorkflowParameters::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // string sequence_file = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_sequence_file();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "SmartPeakServer.WorkflowParameters.sequence_file"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* WorkflowParameters::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:SmartPeakServer.WorkflowParameters)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string sequence_file = 1;
  if (this->sequence_file().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_sequence_file().data(), static_cast<int>(this->_internal_sequence_file().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "SmartPeakServer.WorkflowParameters.sequence_file");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_sequence_file(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:SmartPeakServer.WorkflowParameters)
  return target;
}

size_t WorkflowParameters::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:SmartPeakServer.WorkflowParameters)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string sequence_file = 1;
  if (this->sequence_file().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_sequence_file());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void WorkflowParameters::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:SmartPeakServer.WorkflowParameters)
  GOOGLE_DCHECK_NE(&from, this);
  const WorkflowParameters* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<WorkflowParameters>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:SmartPeakServer.WorkflowParameters)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:SmartPeakServer.WorkflowParameters)
    MergeFrom(*source);
  }
}

void WorkflowParameters::MergeFrom(const WorkflowParameters& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:SmartPeakServer.WorkflowParameters)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.sequence_file().size() > 0) {
    _internal_set_sequence_file(from._internal_sequence_file());
  }
}

void WorkflowParameters::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:SmartPeakServer.WorkflowParameters)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void WorkflowParameters::CopyFrom(const WorkflowParameters& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:SmartPeakServer.WorkflowParameters)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool WorkflowParameters::IsInitialized() const {
  return true;
}

void WorkflowParameters::InternalSwap(WorkflowParameters* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  sequence_file_.Swap(&other->sequence_file_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}

::PROTOBUF_NAMESPACE_ID::Metadata WorkflowParameters::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

class WorkflowStatus::_Internal {
 public:
};

WorkflowStatus::WorkflowStatus(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:SmartPeakServer.WorkflowStatus)
}
WorkflowStatus::WorkflowStatus(const WorkflowStatus& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  status_code_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_status_code().empty()) {
    status_code_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_status_code(), 
      GetArena());
  }
  session_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_session_id().empty()) {
    session_id_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_session_id(), 
      GetArena());
  }
  path_to_results_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_path_to_results().empty()) {
    path_to_results_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_path_to_results(), 
      GetArena());
  }
  progress_in_percent_ = from.progress_in_percent_;
  // @@protoc_insertion_point(copy_constructor:SmartPeakServer.WorkflowStatus)
}

void WorkflowStatus::SharedCtor() {
status_code_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
session_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
path_to_results_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
progress_in_percent_ = 0;
}

WorkflowStatus::~WorkflowStatus() {
  // @@protoc_insertion_point(destructor:SmartPeakServer.WorkflowStatus)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void WorkflowStatus::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  status_code_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  session_id_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  path_to_results_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void WorkflowStatus::ArenaDtor(void* object) {
  WorkflowStatus* _this = reinterpret_cast< WorkflowStatus* >(object);
  (void)_this;
}
void WorkflowStatus::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void WorkflowStatus::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void WorkflowStatus::Clear() {
// @@protoc_insertion_point(message_clear_start:SmartPeakServer.WorkflowStatus)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  status_code_.ClearToEmpty();
  session_id_.ClearToEmpty();
  path_to_results_.ClearToEmpty();
  progress_in_percent_ = 0;
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* WorkflowStatus::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // int32 progress_in_percent = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          progress_in_percent_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string status_code = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_status_code();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "SmartPeakServer.WorkflowStatus.status_code"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string session_id = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          auto str = _internal_mutable_session_id();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "SmartPeakServer.WorkflowStatus.session_id"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string path_to_results = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 34)) {
          auto str = _internal_mutable_path_to_results();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "SmartPeakServer.WorkflowStatus.path_to_results"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* WorkflowStatus::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:SmartPeakServer.WorkflowStatus)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 progress_in_percent = 1;
  if (this->progress_in_percent() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(1, this->_internal_progress_in_percent(), target);
  }

  // string status_code = 2;
  if (this->status_code().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_status_code().data(), static_cast<int>(this->_internal_status_code().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "SmartPeakServer.WorkflowStatus.status_code");
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_status_code(), target);
  }

  // string session_id = 3;
  if (this->session_id().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_session_id().data(), static_cast<int>(this->_internal_session_id().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "SmartPeakServer.WorkflowStatus.session_id");
    target = stream->WriteStringMaybeAliased(
        3, this->_internal_session_id(), target);
  }

  // string path_to_results = 4;
  if (this->path_to_results().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_path_to_results().data(), static_cast<int>(this->_internal_path_to_results().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "SmartPeakServer.WorkflowStatus.path_to_results");
    target = stream->WriteStringMaybeAliased(
        4, this->_internal_path_to_results(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:SmartPeakServer.WorkflowStatus)
  return target;
}

size_t WorkflowStatus::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:SmartPeakServer.WorkflowStatus)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string status_code = 2;
  if (this->status_code().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_status_code());
  }

  // string session_id = 3;
  if (this->session_id().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_session_id());
  }

  // string path_to_results = 4;
  if (this->path_to_results().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_path_to_results());
  }

  // int32 progress_in_percent = 1;
  if (this->progress_in_percent() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_progress_in_percent());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void WorkflowStatus::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:SmartPeakServer.WorkflowStatus)
  GOOGLE_DCHECK_NE(&from, this);
  const WorkflowStatus* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<WorkflowStatus>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:SmartPeakServer.WorkflowStatus)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:SmartPeakServer.WorkflowStatus)
    MergeFrom(*source);
  }
}

void WorkflowStatus::MergeFrom(const WorkflowStatus& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:SmartPeakServer.WorkflowStatus)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.status_code().size() > 0) {
    _internal_set_status_code(from._internal_status_code());
  }
  if (from.session_id().size() > 0) {
    _internal_set_session_id(from._internal_session_id());
  }
  if (from.path_to_results().size() > 0) {
    _internal_set_path_to_results(from._internal_path_to_results());
  }
  if (from.progress_in_percent() != 0) {
    _internal_set_progress_in_percent(from._internal_progress_in_percent());
  }
}

void WorkflowStatus::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:SmartPeakServer.WorkflowStatus)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void WorkflowStatus::CopyFrom(const WorkflowStatus& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:SmartPeakServer.WorkflowStatus)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool WorkflowStatus::IsInitialized() const {
  return true;
}

void WorkflowStatus::InternalSwap(WorkflowStatus* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  status_code_.Swap(&other->status_code_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  session_id_.Swap(&other->session_id_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  path_to_results_.Swap(&other->path_to_results_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  swap(progress_in_percent_, other->progress_in_percent_);
}

::PROTOBUF_NAMESPACE_ID::Metadata WorkflowStatus::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace SmartPeakServer
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::SmartPeakServer::WorkflowParameters* Arena::CreateMaybeMessage< ::SmartPeakServer::WorkflowParameters >(Arena* arena) {
  return Arena::CreateMessageInternal< ::SmartPeakServer::WorkflowParameters >(arena);
}
template<> PROTOBUF_NOINLINE ::SmartPeakServer::WorkflowStatus* Arena::CreateMaybeMessage< ::SmartPeakServer::WorkflowStatus >(Arena* arena) {
  return Arena::CreateMessageInternal< ::SmartPeakServer::WorkflowStatus >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
