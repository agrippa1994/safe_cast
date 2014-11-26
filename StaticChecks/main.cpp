#include <iostream>
#include <vector>
#include <type_traits>

template<bool>
struct Check;

template<>
struct Check<true> 
{
	typedef bool type;
	enum { value = true };
};

template<bool B>
struct CastIfSameSizeHelper
{
	enum { value = B };
};

template<class T, class U>
struct CastIfSameSize
{
	enum { value = CastIfSameSizeHelper<sizeof(T) == sizeof(U)>::value };
};

template<typename T, bool>
struct DefaultReferencePolicyHelper;

template<typename T>
struct DefaultReferencePolicyHelper<T, true>
{
	typedef T type;
};

template<typename T>
struct DefaultReferencePolicyHelper<T, false>
{
	typedef T& type;
};

template<typename T>
struct DefaultReferencePolicy
{
	enum { value = std::is_arithmetic<T>::value };
	typedef typename DefaultReferencePolicyHelper<T, std::is_arithmetic<T>::value>::type type;
};

template
<
	typename T, 
	typename U, 
	template <class, class> class CheckPolicy = CastIfSameSize,
	template <class> class ReferencePolicy = DefaultReferencePolicy,
	typename CastingType = ReferencePolicy<T>::type
>
CastingType safe_cast(U& t)
{
	const bool b = Check<CheckPolicy<T, U>::value>::value;
	return (CastingType) t;
}

int main()
{
	// Checking an integral type
	{
		std::cout << "Checking a conversation of an integral type" << std::endl;
		float s = 12.0;
		std::cout << safe_cast<int>(s) << std::endl;
		std::cout << std::endl;
	}

	// Check a structure
	{
		struct NormalPos 
		{
			float x, y, z;
		};

		struct ReversePos
		{
			float z, y, x;
		};

		NormalPos np = { 1235.2345f, 665.198f, 860.83496f };
		auto& rp = safe_cast<ReversePos>(np);
		np.x = 2.456f;

		std::cout << "Checking a structure via a reference" << std::endl;
		std::cout << (np.x == rp.z) << std::endl;
		std::cout << (np.y == rp.y) << std::endl;
		std::cout << (np.z == rp.x) << std::endl;
		std::cout << std::endl;
	}

	// Try to cast a structure to an integral or float type with the same size
	{
		struct X
		{
			float f;
		};

		float g = 5456.89f;
		auto& x = safe_cast<X>(g); // Works because the structure has the same size as the float type

		std::cout << "Try to cast a structure to an integral or float type with the same size" << std::endl;
		std::cout << (x.f == g) << std::endl;
		std::cout << std::endl;
	}

	// Try to cast an integral or float type to a structure with a different size
	{
		struct Pos
		{
			Pos(float x, float y) : x(x), y(y) { }
			float x, y;
		};

		
		float f = safe_cast<float>(Pos({ 346.4365f, 496.843f })); // Error while compiling!
		std::cout << std::endl;
	}

	std::cin.get();
}