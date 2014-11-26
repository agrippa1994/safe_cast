#include <iostream>
#include <vector>
#include <string>
#include <type_traits>
#include <tuple>

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
constexpr CastingType safe_cast(U&& u, typename Check<CheckPolicy<T, U>::value>::type * = nullptr)
{
	return (CastingType) u;
}

int main()
{
	// Checking an integral type
	{
		std::cout << "Checking a conversation of an integral type" << std::endl;
		float s = 12.45f;
		std::cout << safe_cast<int>(s) << std::endl; // Will be called in runtime due to the dynamic value
		std::cout << safe_cast<float, int>(5) << std::endl;
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

	// Working with tuples
	{
		struct Pos3D
		{
			Pos3D(float x, float y, float z) : x(x), y(y), z(z) { }

			void print()
			{
				std::cout << x << ' ' << y << ' ' << z << std::endl;
			}

			float x, y, z;
		};

		std::cout << "Working with tuples" << std::endl;

		// Create Position-Object
		Pos3D p(346.4365f, 4562.46765f, 45.0f);
		p.print();

		// Reference the Position Object to a tuple
		std::tuple<float, float, float>& posTuple = safe_cast<decltype(posTuple)>(p);

		// Change the tuple's content
		std::get<0>(posTuple) = 5.46f;
		std::get<1>(posTuple) = 938.509f;
		std::get<2>(posTuple) = 1964.3959f;

		// Print the tuple's content via a referenced Pos3D-Object
		safe_cast<Pos3D>(posTuple).print();
		p.print();

		std::cout << std::endl;
	}

	std::cin.get();
}