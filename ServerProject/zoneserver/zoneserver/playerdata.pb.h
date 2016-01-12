// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: playerdata.proto

#ifndef PROTOBUF_playerdata_2eproto__INCLUDED
#define PROTOBUF_playerdata_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_playerdata_2eproto();
void protobuf_AssignDesc_playerdata_2eproto();
void protobuf_ShutdownFile_playerdata_2eproto();

class PlayerData;

// ===================================================================

class PlayerData : public ::google::protobuf::Message {
 public:
  PlayerData();
  virtual ~PlayerData();

  PlayerData(const PlayerData& from);

  inline PlayerData& operator=(const PlayerData& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const PlayerData& default_instance();

  void Swap(PlayerData* other);

  // implements Message ----------------------------------------------

  PlayerData* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const PlayerData& from);
  void MergeFrom(const PlayerData& from);
  void Clear();
  bool IsInitialized() const;

  PlayerData*  CreateByString(const char*  SerializeStr) const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required uint32 sid = 1;
  inline bool has_sid() const;
  inline void clear_sid();
  static const int kSidFieldNumber = 1;
  inline ::google::protobuf::uint32 sid() const;
  inline void set_sid(::google::protobuf::uint32 value);

  // required string PlayerName = 2;
  inline bool has_playername() const;
  inline void clear_playername();
  static const int kPlayerNameFieldNumber = 2;
  inline const ::std::string& playername() const;
  inline void set_playername(const ::std::string& value);
  inline void set_playername(const char* value);
  inline void set_playername(const char* value, size_t size);
  inline ::std::string* mutable_playername();
  inline ::std::string* release_playername();
  inline void set_allocated_playername(::std::string* playername);

  // required bool bisnewplayer = 3;
  inline bool has_bisnewplayer() const;
  inline void clear_bisnewplayer();
  static const int kBisnewplayerFieldNumber = 3;
  inline bool bisnewplayer() const;
  inline void set_bisnewplayer(bool value);

  // repeated int32 SkillIndex = 4;
  inline int skillindex_size() const;
  inline void clear_skillindex();
  static const int kSkillIndexFieldNumber = 4;
  inline ::google::protobuf::int32 skillindex(int index) const;
  inline void set_skillindex(int index, ::google::protobuf::int32 value);
  inline void add_skillindex(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      skillindex() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_skillindex();

  // @@protoc_insertion_point(class_scope:PlayerData)
 private:
  inline void set_has_sid();
  inline void clear_has_sid();
  inline void set_has_playername();
  inline void clear_has_playername();
  inline void set_has_bisnewplayer();
  inline void clear_has_bisnewplayer();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* playername_;
  ::google::protobuf::uint32 sid_;
  bool bisnewplayer_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > skillindex_;
  friend void  protobuf_AddDesc_playerdata_2eproto();
  friend void protobuf_AssignDesc_playerdata_2eproto();
  friend void protobuf_ShutdownFile_playerdata_2eproto();

  void InitAsDefaultInstance();
  static PlayerData* default_instance_;
};
// ===================================================================


// ===================================================================

// PlayerData

// required uint32 sid = 1;
inline bool PlayerData::has_sid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void PlayerData::set_has_sid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void PlayerData::clear_has_sid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void PlayerData::clear_sid() {
  sid_ = 0u;
  clear_has_sid();
}
inline ::google::protobuf::uint32 PlayerData::sid() const {
  // @@protoc_insertion_point(field_get:PlayerData.sid)
  return sid_;
}
inline void PlayerData::set_sid(::google::protobuf::uint32 value) {
  set_has_sid();
  sid_ = value;
  // @@protoc_insertion_point(field_set:PlayerData.sid)
}

// required string PlayerName = 2;
inline bool PlayerData::has_playername() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void PlayerData::set_has_playername() {
  _has_bits_[0] |= 0x00000002u;
}
inline void PlayerData::clear_has_playername() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void PlayerData::clear_playername() {
  if (playername_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    playername_->clear();
  }
  clear_has_playername();
}
inline const ::std::string& PlayerData::playername() const {
  // @@protoc_insertion_point(field_get:PlayerData.PlayerName)
  return *playername_;
}
inline void PlayerData::set_playername(const ::std::string& value) {
  set_has_playername();
  if (playername_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    playername_ = new ::std::string;
  }
  playername_->assign(value);
  // @@protoc_insertion_point(field_set:PlayerData.PlayerName)
}
inline void PlayerData::set_playername(const char* value) {
  set_has_playername();
  if (playername_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    playername_ = new ::std::string;
  }
  playername_->assign(value);
  // @@protoc_insertion_point(field_set_char:PlayerData.PlayerName)
}
inline void PlayerData::set_playername(const char* value, size_t size) {
  set_has_playername();
  if (playername_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    playername_ = new ::std::string;
  }
  playername_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:PlayerData.PlayerName)
}
inline ::std::string* PlayerData::mutable_playername() {
  set_has_playername();
  if (playername_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    playername_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:PlayerData.PlayerName)
  return playername_;
}
inline ::std::string* PlayerData::release_playername() {
  clear_has_playername();
  if (playername_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = playername_;
    playername_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void PlayerData::set_allocated_playername(::std::string* playername) {
  if (playername_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete playername_;
  }
  if (playername) {
    set_has_playername();
    playername_ = playername;
  } else {
    clear_has_playername();
    playername_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:PlayerData.PlayerName)
}

// required bool bisnewplayer = 3;
inline bool PlayerData::has_bisnewplayer() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void PlayerData::set_has_bisnewplayer() {
  _has_bits_[0] |= 0x00000004u;
}
inline void PlayerData::clear_has_bisnewplayer() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void PlayerData::clear_bisnewplayer() {
  bisnewplayer_ = false;
  clear_has_bisnewplayer();
}
inline bool PlayerData::bisnewplayer() const {
  // @@protoc_insertion_point(field_get:PlayerData.bisnewplayer)
  return bisnewplayer_;
}
inline void PlayerData::set_bisnewplayer(bool value) {
  set_has_bisnewplayer();
  bisnewplayer_ = value;
  // @@protoc_insertion_point(field_set:PlayerData.bisnewplayer)
}

// repeated int32 SkillIndex = 4;
inline int PlayerData::skillindex_size() const {
  return skillindex_.size();
}
inline void PlayerData::clear_skillindex() {
  skillindex_.Clear();
}
inline ::google::protobuf::int32 PlayerData::skillindex(int index) const {
  // @@protoc_insertion_point(field_get:PlayerData.SkillIndex)
  return skillindex_.Get(index);
}
inline void PlayerData::set_skillindex(int index, ::google::protobuf::int32 value) {
  skillindex_.Set(index, value);
  // @@protoc_insertion_point(field_set:PlayerData.SkillIndex)
}
inline void PlayerData::add_skillindex(::google::protobuf::int32 value) {
  skillindex_.Add(value);
  // @@protoc_insertion_point(field_add:PlayerData.SkillIndex)
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
PlayerData::skillindex() const {
  // @@protoc_insertion_point(field_list:PlayerData.SkillIndex)
  return skillindex_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
PlayerData::mutable_skillindex() {
  // @@protoc_insertion_point(field_mutable_list:PlayerData.SkillIndex)
  return &skillindex_;
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_playerdata_2eproto__INCLUDED
