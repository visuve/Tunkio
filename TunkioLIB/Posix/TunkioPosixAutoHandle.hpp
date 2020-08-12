#pragma once

namespace Tunkio
{
	constexpr int InvalidFileDescriptor = -1;

	// https://en.wikipedia.org/wiki/File_descriptor
	class PosixAutoHandle
	{
	public:
		PosixAutoHandle() = default;
		PosixAutoHandle(int handle);
		~PosixAutoHandle();

		void Reset(int handle = InvalidFileDescriptor);
		const int Descriptor() const;
		bool IsValid() const;
	private:
		int m_handle = InvalidFileDescriptor;
	};
}