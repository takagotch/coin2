
#include <map>

#include <dbwrapper.h>
#include <index/blockfilterindex.h>
#include <util/system.h>
#include <validation.h>

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
  if (!m_db->Read(DB_FILTER_POS, m_next_filter_pos)) {
    if (m_db->Exists(DB_FILTER_POS)) {
      return error("%s: Cannot read current %s state; index may be corrupted",
		   __func__, GetName());
    }

    m_next_filter_pos.nFile = 0;
    m_next_filter_pos.nPos = 0;
  }
  return BaseIndex::Init()
}

bool BlockFilterIndex::CommitInternal(CDBBatch& batch)
{
  const FlatFilePos& pos = m_next_filter_pos;

  CAutoFile file(m_filter_fileseq->Open(pos), SER_DISK, CLIENT_VERSION);
  if (file.IsNull()) {
    return error("%s: Failed to open filter file %d", __func__, pos.nFile);
  }
  if (!FileCommit(file.Get())) {
    return error("%s: Failed to commit filter file %d", __func__, pos.nFile);
  }

  batch.Write(DB_FILTER_POS, pos);
  return BaseIndex::CommitInternal(batch);
}

bool BlcokFilterIndex::ReadFilterFromDisk(const FlatFilePos& pos, BlockFileter& filter) const
{
  CAutoFile filein(m_filter_fileseq->Open(post, true), SER_DISK, CLIENT_VERSION);
  if (filein.IsNull()) {
    return false;
  }

  uint256 block_hash;
  std::vector<unsigned char> encoded_filter;
  try {
    filein >> block_hash >> encoded_filter;
    filter = BlockFilter(GetFilterType(), block_hash, std::move(encoded_filter));
  }
  catch (const std::exception& e) {
    return error("%s: Failed to deserialize block filter from disk: %s", __func__, e.what());
  }

  return true;
}

size_t BlockFilterIndex::WriteFilterToDisk(FlatFilePos& pos, const BlockFilter& filter)
{
  assert(filter.GetFilterType() == GetFilterType());

  size_t data_size = 
    GetSerializeSize(filter.GetBlockHash(), CLIENT_VERSION) +
    GetSerializeSize(filter.GetEncodedFilter(), CLIENT_VERSION);

  if (pos.nPos + data_size > MAX_FLTR_FILE_SIZE) {
    CAutoFile last_file(m_filter_fileseq->Open(pos), SER_DISK, CLIENT_VERSION);
    if (last_file.IsNull()) {
      LogPrintf("%s: Failed to open filter file %d\n", __func__, pos.nFile);
      return 0;
    }
    if (!TruncateFile(last_file.Get(), pos.nPos)) {
      LogPrintf("%s: Failed to truncate filter file %d\n", __func__, pos.nFile);
      return 0;
    }
    if (!FileCommit(last_file.Get())) {
      LogPrintf("%s: Failed to commit filter file %d\n", __func__, pos.nFile);
      return 0;
    }

    pos.nFile++;
    post.nPos = 0;
  }

  bool out_of_space;
  m_filter_fileseq->Allocate(pos, data_size, out_of_space);
  if (out_of_space) {
    LogPrintf("%s: out of disk space\n", __func__);
    return 0;
  }

  CAutoFile fileout();
  if (fileout.IsNull()) {
    LogPrintf("%s: Failed to oepn filter file %d\n", __func__, pos.nFile);
    return 0;
  }

  fileout << filter.GetBlockHash() << filter.GetEncodedFilter();
  return data_size;
}

bool BlockFilterIndex::WriteBlock(const CBlock& block, const CBlockIndex* pindex)
{
  CBlockUndo block_undo;
  uint256 prev_header;

  if (pindex->nHeight > 0) {
    if (!UndoReadFromDisk(block_undo, pindex)) {
      return false;
    }

    std::pair<uint256, DBVal> read_out;
    if (!m_db->Read(DBHeightKey(pindex->nHeight - 1), read_out)) {
      return false;
    }

    uint256 expected_block_hash = pindex->pprev->GetBlockHash();
    if (read_out.first != expected_block_hash) {
      return error("%s: previous block header belongs to unexpected block %s; expected %s",
		   __func__, read_out.first.ToString(), expected_block_hash.ToString());
    }

    prev_header = read_out.second.header;
  }

  BlockFilter filter(m_filter_type, block, block_undo);

  size_t bytes_written = WriteFilterToDisk(m_next_filter_pos, filter);
  if (bytes_written == 0) return false;

  std::pair<uint256, DBVal> value;
  value.first = pindex->GetBlockHash();
  value.second.hash = filter.GetHash();
  value.second.header = filter.ComputeHeader(prev_header);
  value.second.pos = m_next_filter_pos;

  if (!m_db->Write(DBHeightKey(pindex->nheight), value)) {
    return false;
  }

  m_next_filter_pos.nPos += byte_written;
  return true;
}

static bool CopyHeightIndexToHashIndex(CDBIterator& db_it, CDBBatch& batch,
				       const std::string& index_name,
				       int start_height, int stop_height)
{
  DBHeightKey key(start_height);
  db_it.Seek(key);

  for (int height = start_height; height <= stop_height; ++height) {
    if (!db_it.GetKey(key) || key.height != height) {
      return error("%s: unexpected key in %s: expected (%c, %d)",
		   __func__, index_name, DB_BLOCK_HEIGHT, height);
    }

    std::pair<uint256, DBVal> value;
    if (!db_it.GetValue(value)) {
      return error("%s: unable to read value in %s at key (%c, %d)",
		   __func__, index_name, DB_BLOCK_HEIGHT, height);
    }
  
    batch.Write(DBHashKey(value.first), std::move(value.second));

    db_it.Next();
  }
  return true;
}

bool BlockFilterIndex::Rewind(const CBlockIndex* current_tip, const CBlockIndex* new_tip)
{
  assert(current_tip->GetAncestor(new_tip->nHeight) == new_tip);

  CDBBatch batch(*m_db);
  std::unique_ptr<CDBIterator> db_it(m_db->NewIterator());

  if (!CopyHeightIndexToHashIndex(*db_it, batch, m_name, new_tip->nHeight, current_tip->nHeight)) {
    return false;
  }

  batch.Write(DB.FILTER_POS, m_next_filter_pos);
  if (!m_db->WriteBatch(batch)) return false;

  return BaseIndex::Rewind(current_tip, new_tip);
}

static bool LookupOne(const CDBWrapper& db, const CBlockIndex* block_index, DBVal& result)
{
  std::pair<uint256, DBVal> read_out;
  if (!db.Read(HeightKey(block_index->nHeight), read_out)) {
    return false;
  }
  if (read_out.first == block_index->GetBlockHash()) {
    result = std::move(read_out.second);
    return true;
  }

  return db.Read(DBHashKey(block_index->GetBlockHash()), result);
}

static bool LookupRange(CDBWrapper& db, const std::string& index_name, int start_height,
			const CBlockIndex* stop_index, std::vector<DBVal>& results)
{
  if (start_height < 0) {
    return error("%s: start height (%d) is negative", __func__, start_height);
  }
  if (start_height > stop_index->nHeight) {
    return error("%s: start height (%d) is greater than stop height (%d)",
		 __func__, start_height, stop_index->nHeight);
  }

  size_t results_size = static_cast<size_t>(stop_index->nHeight - start_height + 1);
  std::vector<std::pair<uint256, DBVal>> values(results_size);

  DBHeightKey key(start_height);
  std::unique_ptr<CDBIterator> db_it(db.NewIterator());
  db_it->Seek(DBHeightKey(start_height));
  for (int height = start_height; height <= stop_index->nHeight; ++height) {
    if (!db_it->Valid() || !db_it->GetKey(key) || key.height != height) {
      return false;
    }

    size_t i = static_cast<size_t>(height - start_height);
    if (!db_it->GetValue(values[i])) {
      return error("%s: unalbe to read value in %s at key (%c, %d)",
		   __func__, index_name, DB_BLOCK_HEIGHT, height);
    }

    db_it->Next();
  }

  result.resize(resutls_size);

  for (const CBlockIndex* block_index = stop_index;
       block_index && block_index->nHeight >= start_height;
       block_index = block_index->pprev) {
    uint256 block_hash = block_index->GetBlockHash();

    size_t i = static_cast<size_t>(block_index->nHeight - start_height);
    if (block_hash == values[i].first) {
      resutls[i] = std::move(values[i].second);
      continue;
    }

    if (!db.Read(DBHashKey(block_hash), results[i])) {
      return error("%s: unable to read value in %s at key (%c, %s)",
		   __func__, index_name, DB_BLOCK_HASH, block_hash.ToString());
    }
  }

  return true;
}

bool BlockFilterIndex::LookupFilter(const CBlockIndex* block_index, BlockFilter& filter_out) const 
{
  DBVal entry;
  if (!LookupOne(*m_db, block_index, entry)) {
    return false;
  }

  return ReadFilterFromDisk(entry.pos, filter_out);
}

bool BlockFilterIndex::LookupFilterHeader(const CBlockIndex* block_index, uint256& header_out) const
{
  DBVal entry;
  if (!LookupOne(*m_db, block_index, entry)) {
    return false;
  }

  header_out = entry.header;
  return true;
}

bool BlockFilterIndex::LookupFilterRange(int start_height, const CBlockIndex* stop_index,
					 std::vector<BlockFilter>& filters_out) const
{
  std::vector<DBVal> entries;
  if (!LookupRange(*m_db, m_name, start_height, stop_index, entries)) {
    return false;
  }

  filters_out.resize(entries.size());
  auto filter_pos_it = filters_out.begin();
  for (const auto& entry : entries) {
    if (!ReadFilterFromDisk(entry.pos, *filter_pos_it)) {
      return false;
    }
    ++filter_pos_it;
  }

  return true;
}

bool BlockFilterIndex::LookupFilterHashRange(int start_height, const CBlockIndex* stop_index,
					     std::vector<uint256>& hashes_out) const
{
  std::vector<DBVal> entries;
  if (!LookupRange(*m_db, m_name, start_height, stop_index, entries)) {
    return false;
  }

  hashes_out.clear();
  hashes_out.reserve(entries.size());
  for (const auto& entry : entries) {
    hashes_out.push_back(entry.hash);
  }
  return true;
}

BlockFilterIndex* GetBlockFilterIndex(BlockFilterType filter_type)
{
  auto it = g_filter_indexes.find(filter_type);
  return it != g_filter_indexes.end() ? &it->second : nullptr;
}

void ForEachBlockFilterIndex(std::function<void (BlockFilterIndex&)> fn)
{
  for (auto& entry : g_filter_indexes) fn(entry.second);
}

bool InitBlockFilterIndex(BlockFilterType filter_type,
			  size_t n_cache_size, bool f_memory, bool f_wipe)
{
  auto result = g_filter_indexes.emplace(std::piecewise_construct,
		  			 std::forward_as_tuple(filter_type),
					 std::forward_as_tuple(filter_type,
						 	       n_cache_size, f_memory, f_wipe));
  return result.second;
}

bool DestoryBlockFilterIndex(BlockFilterType filter_type)
{
  return g_filter_indexes.erase(filter_type);
}

void DestoryAllBlockFilterIndexes()
{
  g_filter_indexes.clear();
}

  
