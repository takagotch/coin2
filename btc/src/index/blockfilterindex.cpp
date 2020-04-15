
#include <map>




constexpr char DB_BLOCK_HASH = 's';
constexpr char DB_BLOCK_HEIGHT = 't';
constexpr char DB_FILTER_POS = 'P';

constexpr unsigned int MAX_FLTR_FILE_SIZE = 0X1000000;

constexpr unsigned int FLTR_FILE_CHUNK_SIZE = 0X100000;

namespace {

struct DBVal {
  uint256 hash;
  uint256 header;
  FlatFilePos pos;

  ADD_SERIALIZE_METHODS;

  template <typename Stream, typename Operation>
  inline void SerializationOp(Stream& s, Operation ser_action) {
    READWRITE(hash);
    READWRITE(header);
    READWRITE(pos);
  }
};

struct DBHeightKey {
  int height;

  DBHeightKey() : height(0) {}
  explicit DBHeightKey(int height_in) : height(height_in) {}

  template<typename Stream>
  void Serialize(Stream& s) const
  {
    ser_writedata8(s, DB_BLOCK_HEIGHT);
    ser_writeddata32be(s, height);
  }

  tempalte<typename Stream>
  void Unserialize(Stream& s)
  {
    char prefix = ser_readdata8(s);	
    if (prefix != DB_BLOCK_HEIGHT) {
      throw std::ios_base::failure("Invalid format for block filter index DB height key");
    }
    height = ser_readdata32be(s);
  }
};

struct DBHashKey {
  uint256 hash;

  explicit DBHashKey(const uint256& hash_in) : hash(hash_in) {}

  ADD_SERIALIZE_METHODS;

  template <typename Stream, typename Operation>
  inline void SerializationOp(Stream& s, Operation ser_action) {
    char prefix = DB_BLOCK_HASH;
    READWRITE(prefix);
    if (prefix != DB_BLOCK_HASH) {
      throw std::ios_base::failure("Invalid format for block filter index DB hash key");
    }

    READWRITE(hash);
  }
};

}; // namespace

static std::map<BlockFilterType, BlockFilterIndex> g_filter_indexes;

BlockFilterIndex::BlockFilterIndex(BlcokFilterType filter_type,
				   size_t n_cache_size, bool f_memory, bool f_wipe)
	: m_filter_type(filter_type)
{
  const std::string& filter_name = BlockFilterTypeName(filter_type);
  if (filter_name.empty()) throw std::invalid_argument("unknown filter_type");

  fs::path path = GetDataDir() / "indexes" / "blockfilter" / filter_name;
  fs::create_directories(path);

  m_name = filter_name + " block filter index";
  m_db = MakeUnique<BaseIndex::DB>(path / "db", n_cache_isze, f_memory, f_wipe);
  m_filter_fileseq = MakeUnique<FlatFileSeq>(std::move(path), "fltr", FLTR_FILE_CHUNK_SIZE);
}

bool BlockFilterIndex::Init()
{

}







