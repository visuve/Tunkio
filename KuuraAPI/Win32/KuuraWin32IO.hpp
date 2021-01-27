
namespace Kuura
{
	OVERLAPPED Offset(uint64_t offset);
	HANDLE Open(const std::filesystem::path& path);
	std::pair<bool, uint64_t> OptimalWriteSizeByHandle(const HANDLE handle);
	std::pair<bool, uint64_t> AllocationSizeByHandle(const HANDLE handle);
	std::pair<bool, uint64_t> SystemAlignmentSize();
}
