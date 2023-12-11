// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "elements/kernel.h"

namespace dnf_composer
{
	namespace element
	{
		Kernel::Kernel()
		{
			circular = true;
			normalized = false;
			kernelRange = { 0, 0 };
		}

		bool Kernel::getCircular() const
		{
			return circular;
		}

		bool Kernel::getNormalized() const
		{
			return normalized;
		}

		std::array<int, 2> Kernel::getKernelRange() const
		{
			return kernelRange;
		}

		std::vector<int> Kernel::getExtIndex() const
		{
			return extIndex;
		}
	}
}
